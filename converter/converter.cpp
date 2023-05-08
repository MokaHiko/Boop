#include <iostream>
#include <filesystem>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <boop.h>

bool convert_image(const std::filesystem::path& input, const std::filesystem::path& output)
{
	int width, height, nChannels;

	// Force rgba
	stbi_uc* data = stbi_load(input.u8string().c_str(), &width, &height, &nChannels, STBI_rgb_alpha);

	if(!data)
	{
		return false;
	}

	int texture_size = width * height * 4;

	boop::TextureInfo texture_info = {};
	texture_info.format = boop::TextureFormat::RGBA8;
	texture_info.compression_mode = boop::CompressionMode::LZ4;
	texture_info.original_file_path = input.string();
	texture_info.pixel_size[0] = width;
	texture_info.pixel_size[1] = height;
	texture_info.texture_size = texture_size;
	boop::AssetFile compressed_image = boop::pack_texture(&texture_info, data);

	stbi_image_free(data);
	boop::save(output.string().c_str(), compressed_image);

	return true;
}

int main(int argc, char** argv)
{
	if(argc != 2) {
		std::cout << "ERROR::Incorrect Usage: " << "boop_converter.exe <assets_dir_path>\n";
		return -1;
	}

	std::filesystem::path filePath{argv[1]};
	std::filesystem::path directory = filePath;

	for(auto& p : std::filesystem::directory_iterator(directory)) {
		std::cout << "converting:" << p.path().filename() << "...";
		bool success = false;

		if(p.path().extension() == ".png") {
			auto new_path = p.path();
			new_path.replace_extension(".boop");
			success = convert_image(p.path(), new_path);
		}

		std::cout << 
		(success ? " success!" : " failed!") << 
		std::endl;
	}
}
