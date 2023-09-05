#include "model.h"
#include <json.hpp>

#include <iostream>
#include <lz4.h>

namespace boop
{
	ModelInfo read_model_info(AssetFile* file)
	{
		ModelInfo info = {};

		nlohmann::json metadata = nlohmann::json::parse(file->json);
		info.mesh_count = metadata["mesh_count"];
		info.original_file_path = metadata["original_file_path"];

		uint64_t offset = 0;
		nlohmann::json mesh_infos = metadata["mesh_infos"];

		int ctr = 0;
		info.mesh_infos.resize(info.mesh_count);
		for(nlohmann::json::iterator it = mesh_infos.begin(); it != mesh_infos.end(); it++)
		{
			// Update mesh info
			info.mesh_infos[ctr].vertex_buffer_size = (*it)["vertex_buffer_size"];
			info.mesh_infos[ctr].index_buffer_size = (*it)["index_buffer_size"];
			info.mesh_infos[ctr].mesh_name = (*it)["mesh_name"];

			// Update global info
			info.vertex_buffer_size += (*it)["vertex_buffer_size"];
			info.index_buffer_size += (*it)["index_buffer_size"];

			ctr++;
		}

		info.vertex_format = metadata["vertex_format"];

		return info;
	}

	void unpack_model(ModelInfo* info, const char* src_buffer, size_t src_size, char* vertex_buffer, char* index_buffer)
	{
		// Create buffer to hold merged buffer
		std::vector<char> decompressed_buffer;
		decompressed_buffer.resize(info->vertex_buffer_size + info->index_buffer_size);

		LZ4_decompress_safe(src_buffer, decompressed_buffer.data(), static_cast<int>(src_size), static_cast<int>(decompressed_buffer.size()));

		// Copy vertex buffer
		memcpy(vertex_buffer, decompressed_buffer.data(), info->vertex_buffer_size);

		// Copy index buffer
		memcpy(index_buffer, decompressed_buffer.data() + info->vertex_buffer_size, info->index_buffer_size);
	}

	AssetFile pack_model(ModelInfo *info, char *vertex_buffer, char *index_buffer) 
	{
		AssetFile file = {};

		file.type[0] = 'M';
		file.type[1] = 'O';
		file.type[2] = 'D';
		file.type[3] = 'L';

		file.version = 1;

		// ~ Meta data
    	nlohmann::json meta_data;
		meta_data["mesh_count"] = info->mesh_count;
		meta_data["mesh_infos"] = nlohmann::json::array();
		meta_data["original_file_path"] = info->original_file_path;
		meta_data["vertex_buffer_size"] = info->vertex_buffer_size;
		meta_data["index_buffer_size"] = info->index_buffer_size;
		meta_data["compression_mode"] = info->compression_mode;
		meta_data["vertex_format"] = info->vertex_format;

		uint64_t vertex_buffer_size = 0;
		uint64_t index_buffer_size = 0;
		file.raw_data.resize(info->vertex_buffer_size + info->index_buffer_size);
		for(uint32_t i = 0; i < info->mesh_count; i++)
		{
			// Create model mesh meta data
			ModelMeshInfo mesh_info = info->mesh_infos[i];

	    	nlohmann::json model_mesh_metadata;
			model_mesh_metadata["mesh_name"] = mesh_info.mesh_name;
        	model_mesh_metadata["vertex_buffer_size"] = mesh_info.vertex_buffer_size;
        	model_mesh_metadata["index_buffer_size"] = mesh_info.index_buffer_size;

			// Copy meta data
			meta_data["mesh_infos"].push_back(model_mesh_metadata);

			// Count size
			vertex_buffer_size += info->mesh_infos[i].vertex_buffer_size; 
			index_buffer_size += info->mesh_infos[i].index_buffer_size;
		}

		memcpy(file.raw_data.data(), vertex_buffer, vertex_buffer_size);
		memcpy(file.raw_data.data() + vertex_buffer_size, index_buffer, index_buffer_size);

		file.json = meta_data.dump();
		return file;
	}
}