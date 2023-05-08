#include "texture.h"

#include <lz4.h>
#include <json.hpp>

namespace boop
{
	TextureInfo read_texture_info(AssetFile* file)
	{
		TextureInfo info = {};

		nlohmann::json texture_metadata = nlohmann::json::parse(file->json);

		std::string format_string = texture_metadata["format"];
		info.format = parse_texture_format(format_string.c_str());

		info.pixel_size[0] = texture_metadata["width"];
		info.pixel_size[1] = texture_metadata["height"];
		info.texture_size = texture_metadata["texture_size"];
		info.original_file_path = texture_metadata["original_file_path"];

		std::string compression_string = texture_metadata["compression_mode"];
		info.compression_mode = parse_compression(compression_string.c_str());
		
		return info;
	}

	void unpack_texture(TextureInfo* info, const char* src_buffer, size_t src_size, char* dst_buffer)
	{
		if(info->compression_mode == CompressionMode::LZ4)
		{
			LZ4_decompress_safe(src_buffer, dst_buffer, src_size, info->texture_size);
		}
		else
		{
			// NO compression
			memcpy(dst_buffer, src_buffer, src_size);
		}
	}
	
	AssetFile pack_texture(TextureInfo* info, void* texture_buffer)
	{
		nlohmann::json texture_metadata;
		texture_metadata["format"] = "RGBA8";
		texture_metadata["width"] = info->pixel_size[0];
		texture_metadata["height"] = info->pixel_size[1];
		texture_metadata["texture_size"] = info->texture_size;
		texture_metadata["original_file_path"] = info->original_file_path;

		// core file header
		AssetFile file;

		// file type
		file.type[0] = 'T';
		file.type[1] = 'E';
		file.type[2] = 'X';

		file.type[3] = 'I'; // Texture Type
		file.version = 1;

		// Compress info into blob
		int compress_staging = LZ4_compressBound(info->texture_size);

		file.raw_data.resize(compress_staging); // resize to upperbound
		int comressed_size = LZ4_compress_default((const char*)texture_buffer, file.raw_data.data(), info->texture_size, compress_staging);
		file.raw_data.resize(comressed_size); // resize to actual

		texture_metadata["compression_mode"] = "LZ4";

		std::string stringified = texture_metadata.dump();
		file.json = stringified;

		return file;
	}
	
	TextureFormat parse_texture_format(const char* f)
	{
        if (strcmp(f, "RGBA8") == 0)
            return TextureFormat::RGBA8;
        else
            return TextureFormat::Unknown;
	}
}