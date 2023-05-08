#ifndef BOOP_H
#define BOOP_H

#pragma once

#include "asset.h"
#include "texture.h"

namespace boop
{
    // Saves a boop asset file
    bool save(const char* path, const AssetFile& file);

    // Loads a boop asset file
    bool load(const char* path, AssetFile& file);
}

#endif