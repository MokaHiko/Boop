#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

#include <string>
#include <stdint.h>

#include "asset.h"

namespace boop
{
    enum class TextureFormat : uint32_t
    {
        Unknown,
        RGBA8
    };

    struct TextureInfo
    {
        TextureFormat format;
        uint64_t texture_size;
        CompressionMode compression_mode;
        uint32_t pixel_size[3];
        std::string original_file_path;
    };

    // Parses texture meta data from asset file
    TextureInfo read_texture_info(AssetFile* file);

    // Decompresses texture data into destination buffer and places meta data in info
    void unpack_texture(TextureInfo* info, const char* src_buffer, size_t src_size, char* dst_buffer);

    // Compress texture into an asset file given parameters in <TexutreInfo>
    AssetFile pack_texture(TextureInfo* info, void* texture_buffer);

    TextureFormat parse_texture_format(const char* f);
}

#endif