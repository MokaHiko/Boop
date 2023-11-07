#include "assimpImpl.h"
#include "boop.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include "model.h"

bool boop::convert_mesh(const std::filesystem::path& input, const std::filesystem::path& output)
{
	return false;
}

bool boop::convert_model(const std::filesystem::path& input, const std::filesystem::path& output)
{
	using namespace Assimp;
	Importer importer;
	DefaultLogger::create("", Assimp::Logger::NORMAL);
	DefaultLogger::get()->info("Assimp Initalized");

	const aiScene* scene = importer.ReadFile(input.string(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	if (scene == nullptr)
	{
		throw std::runtime_error("Failed to load: " + input.string());
		return false;
	}

	auto start = std::chrono::high_resolution_clock::now();
	AssimpLoader<Vertex_F32_PNCV>* model_loader = new AssimpLoader<Vertex_F32_PNCV>();
	model_loader->load_scene(scene);
	auto end = std::chrono::high_resolution_clock::now();

	auto diff = end - start;
	std::cout << "Model loading took" << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	ModelInfo info = {};
	info.mesh_count = model_loader->mesh_count();
	info.mesh_infos = model_loader->mesh_infos;

	info.material_count = model_loader->material_count();
	info.materials = model_loader->materials;

	info.vertex_format = VertexFormat::F32_PNCV;
	info.vertex_buffer_size = model_loader->vertex_count() * sizeof(Vertex_F32_PNCV);
	info.index_buffer_size = model_loader->index_count() * sizeof(uint32_t);
	info.compression_mode = CompressionMode::LZ4;
	info.original_file_path = input.string();

	// Compress model as asset file
	try
	{
		start = std::chrono::high_resolution_clock::now();
		boop::AssetFile compressed_model = pack_model(&info,
			(char*)model_loader->vertex_data(),
			(char*)model_loader->index_data());
		end = std::chrono::high_resolution_clock::now();
		diff = end - start;

		std::cout << "Model compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;
		delete model_loader;
		DefaultLogger::kill();

		return boop::save(output.string().c_str(), compressed_model);
	}
	catch (std::exception e)
	{
		DefaultLogger::get()->info(e.what());

		delete model_loader;
		DefaultLogger::kill();
		return false;
	}
}