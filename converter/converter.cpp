#include <iostream>
#include <filesystem>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <tiny_obj_loader.h>

#include <boop.h>
#include <optional>


void pack_vertex(boop::Vertex_F32_PNCV& new_vert, tinyobj::real_t vx, tinyobj::real_t vy, tinyobj::real_t vz, tinyobj::real_t nx, tinyobj::real_t ny, tinyobj::real_t nz, tinyobj::real_t ux, tinyobj::real_t uy)
{
	new_vert.position[0] = vx;
	new_vert.position[1] = vy;
	new_vert.position[2] = vz;

	new_vert.normal[0] = nx;
	new_vert.normal[1] = ny;
	new_vert.normal[2] = nz;

	new_vert.uv[0] = ux;
	new_vert.uv[1] = 1 - uy;
}

template<typename VertexFormat>
void extract_mesh_from_obj(tinyobj::attrib_t& attrb, std::vector<tinyobj::shape_t>& shapes, std::vector<VertexFormat>& vertices, std::vector<uint32_t>& indices)
{
	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {

		// Loop over faces (polygons)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

			// Loop over vertices
			int fv = 3; // Hardcode triangles
			for (size_t v = 0; v < fv; v++) {
				// Access to <VertexFormat> 
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				// Get vertex attribs
				tinyobj::real_t vx = attrb.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrb.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrb.vertices[3 * idx.vertex_index + 2];

				tinyobj::real_t nx = attrb.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrb.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrb.normals[3 * idx.normal_index + 2];

				tinyobj::real_t ux = attrb.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t uy = attrb.texcoords[2 * idx.texcoord_index + 1];

				// Store as passed vertex format
				VertexFormat new_vert;
				pack_vertex(new_vert, vx, vy, vz, nx, ny, nz, ux, uy);

				indices.push_back(vertices.size());
				vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}
}

bool convert_mesh(const std::filesystem::path& input, const std::filesystem::path& output)
{
	// Attrib contains the assets vertex(F32_PNCV) arrays
	tinyobj::attrib_t attrib;

	// Contains info for each shape in obj
	std::vector<tinyobj::shape_t> shapes;

	// Materials of each shape (TODO: implement material asseets) 
	std::vector<tinyobj::material_t> materials;

	// Errors and profiling
	std::string warn;
	std::string error;

	// Load obj and profile
	auto pngstart = std::chrono::high_resolution_clock::now();
	tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, input.string().c_str(), nullptr);
	auto diff = std::chrono::high_resolution_clock::now() - pngstart;

	std::cout << "Obj took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!error.empty()) {
		std::cerr << error << std::endl;
		return false;
	}

	using VertexFormat = boop::Vertex_F32_PNCV;
	auto vertex_format_enum = boop::VertexFormat::F32_PNCV;

	std::vector<VertexFormat> vertices;
	std::vector<uint32_t> indices;

	extract_mesh_from_obj(attrib, shapes, vertices, indices);

	boop::MeshInfo mesh_info = {};
	mesh_info.vertex_format = vertex_format_enum;
	mesh_info.vertex_buffer_size = vertices.size() * sizeof(VertexFormat);
	mesh_info.index_buffer_size = indices.size() * sizeof(uint32_t);
	mesh_info.compression_mode = boop::CompressionMode::LZ4;
	mesh_info.original_file_path = input.string();

	// Compress mesh as asset file
	auto start = std::chrono::high_resolution_clock::now();
	boop::AssetFile compressed_mesh = boop::pack_mesh(&mesh_info, (char*)vertices.data(), (char*)indices.data());
	auto  end = std::chrono::high_resolution_clock::now();

	diff = end - start;
	std::cout << "compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	return boop::save(output.string().c_str(), compressed_mesh);
}

bool convert_image(const std::filesystem::path& input, const std::filesystem::path& output)
{
	int width, height, nChannels;

	// Force rgba
	stbi_uc* data = stbi_load(input.u8string().c_str(), &width, &height, &nChannels, STBI_rgb_alpha);

	if (!data)
	{
		return false;
	}

	int texture_size = width * height * 4;

	boop::TextureInfo texture_info = {};
	texture_info.format = boop::TextureFormat::RGBA8;
	texture_info.compression_mode = boop::CompressionMode::LZ4;
	texture_info.original_file_path = input.string();
	texture_info.pixel_size[0] = width;
	texture_info.pixel_size[1] = height;
	texture_info.texture_size = texture_size;
	boop::AssetFile compressed_image = boop::pack_texture(&texture_info, data);

	stbi_image_free(data);

	return boop::save(output.string().c_str(), compressed_image);
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "ERROR::Incorrect Usage: " << "boop_converter.exe <assets_dir_path>\n";
		return -1;
	}

	std::filesystem::path filePath{ argv[1] };
	std::filesystem::path directory = filePath;

	for (auto& p : std::filesystem::directory_iterator(directory)) {
		std::cout << "converting:" << p.path().filename() << "...\n";

		std::optional<bool> success;

		if (p.path().extension() == ".png") {
			auto new_path = p.path();
			new_path.replace_extension(".boop_png");
			success = convert_image(p.path(), new_path);
		}

		if (p.path().extension() == ".obj") {
			auto new_path = p.path();
			new_path.replace_extension(".boop_obj");
			success = convert_mesh(p.path(), new_path);
		}

		// Check if file format is valid
		if (success.has_value()) {
			std::cout <<
			(success ? " success!" : " failed!") <<
			std::endl << 
			std::endl;
		}
	}
}
