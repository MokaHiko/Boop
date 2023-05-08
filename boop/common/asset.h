#ifndef ASSET_H
#define ASSET_H

#pragma once

#include <string>
#include <vector>

namespace boop
{
    struct AssetFile
    {
        char type[4];
        int version;
        std::string json;
        std::vector<char> raw_data;
    };

    enum class CompressionMode : uint32_t
    {
        None,
        LZ4
    };

    CompressionMode parse_compression(const char *f);
}

#endif