#include "tinyObjImpl.h"
#include <chrono>
#include <boop.h>

template<typename VertexFormat>
void boop::extract_mesh_from_obj(tinyobj::attrib_t &attrb, std::vector<tinyobj::shape_t> &shapes, std::vector<VertexFormat> &vertices, std::vector<uint32_t> &indices)
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

bool boop::convert_mesh(const std::filesystem::path &input, const std::filesystem::path &output)
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
	boop::AssetFile compressed_mesh = boop::pack_mesh(&mesh_info, (char *)vertices.data(), (char *)indices.data());
	auto  end = std::chrono::high_resolution_clock::now();

	diff = end - start;
	std::cout << "compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	return boop::save(output.string().c_str(), compressed_mesh);
}