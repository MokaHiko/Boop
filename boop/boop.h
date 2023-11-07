#ifndef BOOP_H
#define BOOP_H

#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

#include "asset.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"

namespace boop
{
    // Outputs mesh into .boop asset file
    bool convert_model(const std::filesystem::path &input, const std::filesystem::path &output);

    // Outputs mesh into .boop asset file
    bool convert_mesh(const std::filesystem::path &input, const std::filesystem::path &output);

    // Outputs mesh into .boop asset file
    bool convert_image(const std::filesystem::path &input, const std::filesystem::path &output);
    
    // Outputs mesh into .boop asset file
	bool convert_image(void* buffer, uint32_t buffer_size, const std::filesystem::path& input, const std::filesystem::path& output);

    // Saves a boop asset file
    bool save(const char *path, const AssetFile &file);

    // Loads a boop asset file
    bool load(const char *path, AssetFile &file);
}

#endif