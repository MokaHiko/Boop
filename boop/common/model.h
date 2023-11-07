#pragma once

#include <stdint.h>
#include <string>

#include "asset.h"
#include "mesh.h"
#include "material.h"

namespace boop {
    struct ModelMeshInfo
    {
        std::string mesh_name;
        uint64_t vertex_buffer_size;
        uint64_t index_buffer_size;

        uint32_t material_index;
        glm::mat4 global_model_matrix;
    };

    struct ModelInfo
    {
        uint64_t mesh_count;
        std::vector<ModelMeshInfo> mesh_infos;

        uint64_t material_count;
        std::vector<Material> materials;

        uint64_t vertex_buffer_size;
        uint64_t index_buffer_size;

        VertexFormat vertex_format;
        CompressionMode compression_mode;

        std::string original_file_path;
    };

    ModelInfo read_model_info(AssetFile* file);

    void unpack_model(ModelInfo* info, const char* src_buffer, size_t src_size, char* vertex_buffer, char* index_buffer);

    // Packs model into asset file given model info and global vertex and index buffer
    AssetFile pack_model(ModelInfo* info, char* vertex_buffer, char* index_buffer);
};
