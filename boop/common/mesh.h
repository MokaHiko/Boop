#ifndef MESH_H
#define MESH_H

#pragma once

#include <cstdint>

#include "asset.h"

namespace boop {
    // Vertex format all in f32
    struct Vertex_F32_PNCV {
        float position[3];
        float color[3];
        float normal[3];
        float uv[2];
    };

    enum class VertexFormat : uint32_t
    {
        Unknown = 0,
        F32_PNCV,
    };

    enum class MeshFormat : uint32_t
    {
        Unknown,
        RGBA8
    };

    struct MeshInfo
    {
        uint64_t vertex_buffer_size;
        uint64_t index_buffer_size;
        VertexFormat vertex_format;
        CompressionMode compression_mode;
        std::string original_file_path;
    };

    MeshInfo read_mesh_info(AssetFile *file);

    void pack_vertex(boop::Vertex_F32_PNCV &new_vert, real_t vx, real_t vy, real_t vz, real_t nx, real_t ny, real_t nz, real_t ux, real_t uy);

    void unpack_mesh(MeshInfo *info, const char *src_buffer, size_t src_size, char *vertex_buffer, char *index_buffer);
    AssetFile pack_mesh(MeshInfo *info, char *vertex_buffer, char *index_buffer);
}
#endif