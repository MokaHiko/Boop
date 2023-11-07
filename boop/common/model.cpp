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
		info.original_file_path = metadata["original_file_path"];

		info.material_count = metadata["material_count"];
		nlohmann::json  materials = metadata["materials"];

		int material_counter = 0;
		info.materials.resize(info.material_count);
		for (nlohmann::json::iterator it = materials.begin(); it != materials.end(); it++)
		{
			Material material = {};
			material.name = (*it)["name"];

			nlohmann::json ambient = (*it)["ambient_color"];
			material.ambient_color = {ambient[0], ambient[1], ambient[2]};

			nlohmann::json diffuse = (*it)["diffuse_color"];
			material.diffuse_color = {diffuse[0], diffuse[1], diffuse[2]};

			nlohmann::json specular = (*it)["specular_color"];
			material.specular_color = {specular[0], specular[1], specular[2]};

			material.diffuse_texture_path = (*it)["diffuse_texture_path"];
			material.specular_texture_path = (*it)["specular_texture_path"];

			info.materials[material_counter++] = material;
		}

		uint64_t offset = 0;
		info.mesh_count = metadata["mesh_count"];
		nlohmann::json mesh_infos = metadata["mesh_infos"];

		int ctr = 0;
		info.mesh_infos.resize(info.mesh_count);

		for (nlohmann::json::iterator it = mesh_infos.begin(); it != mesh_infos.end(); it++)
		{
			// Update mesh info
			info.mesh_infos[ctr].vertex_buffer_size = (*it)["vertex_buffer_size"];
			info.mesh_infos[ctr].index_buffer_size = (*it)["index_buffer_size"];
			info.mesh_infos[ctr].mesh_name = (*it)["mesh_name"];
			info.mesh_infos[ctr].material_index = (*it)["material_index"];

			// TODO: Pack better
			nlohmann::json global_model_matrix = (*it)["global_model_matrix"];
			int model_matrix_ctr = 0;
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					info.mesh_infos[ctr].global_model_matrix[i][j] = global_model_matrix[model_matrix_ctr++];
				}
			}

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

	AssetFile pack_model(ModelInfo* info, char* vertex_buffer, char* index_buffer)
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

		meta_data["material_count"] = info->material_count;
		meta_data["materials"] = nlohmann::json::array();

		meta_data["original_file_path"] = info->original_file_path;
		meta_data["vertex_buffer_size"] = info->vertex_buffer_size;
		meta_data["index_buffer_size"] = info->index_buffer_size;
		meta_data["compression_mode"] = info->compression_mode;
		meta_data["vertex_format"] = info->vertex_format;

		uint64_t vertex_buffer_size = 0;
		uint64_t index_buffer_size = 0;

		// Pack materials
		for (uint32_t i = 0; i < info->material_count; i++)
		{
			nlohmann::json model_material;
			model_material["name"] = info->materials[i].name;

			glm::vec3 ambient = info->materials[i].ambient_color;
			model_material["ambient_color"] = {ambient.x, ambient.y, ambient.z};

			glm::vec3 diffuse = info->materials[i].diffuse_color;
			model_material["diffuse_color"] = {diffuse.x, diffuse.y, diffuse.z};

			glm::vec3 specular = info->materials[i].specular_color;
			model_material["specular_color"] = {specular.x, specular.y, specular.z};

			model_material["diffuse_texture_path"] = info->materials[i].diffuse_texture_path.c_str();
			model_material["specular_texture_path"] = info->materials[i].specular_texture_path.c_str();
			
			meta_data["materials"].push_back(model_material);
		}

		// Pack mesh data
		file.raw_data.resize(info->vertex_buffer_size + info->index_buffer_size);
		for (uint32_t i = 0; i < info->mesh_count; i++)
		{
			// Create model mesh meta data
			ModelMeshInfo mesh_info = info->mesh_infos[i];

			nlohmann::json model_mesh_metadata;
			model_mesh_metadata["mesh_name"] = mesh_info.mesh_name;
			model_mesh_metadata["vertex_buffer_size"] = mesh_info.vertex_buffer_size;
			model_mesh_metadata["index_buffer_size"] = mesh_info.index_buffer_size;
			model_mesh_metadata["material_index"] = mesh_info.material_index;

			model_mesh_metadata["global_model_matrix"] = nlohmann::json::array();

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					 model_mesh_metadata["global_model_matrix"].push_back(mesh_info.global_model_matrix[i][j]);
				}
			}

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