#include <boop.h>

#include <optional>

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "ERROR::Incorrect Usage: " << "boop_converter.exe <assets_dir_path>\n";
		return -1;
	}

	std::filesystem::path filePath{ argv[1] };
	std::filesystem::path directory = filePath;

	for (auto& p : std::filesystem::directory_iterator(directory)) {
		std::cout << "converting:" << p.path().filename() << "...\n";

		std::optional<bool> success;

		try
		{
		auto new_path = p.path();
		new_path.replace_extension(".bp");

		if (p.path().extension() == ".png") 
		{
			success = boop::convert_image(p.path(), new_path);
		}

		if (p.path().extension() == ".jpg") 
		{
			success = boop::convert_image(p.path(), new_path);
		}

		if (p.path().extension() == ".obj") 
		{
			success = boop::convert_mesh(p.path(), new_path);
		}

		if (p.path().extension() == ".fbx") 
		{
			success = boop::convert_model(p.path(), new_path);
		}

		// Check if file format is valid
		if (success.has_value()) {
			std::cout << 
			(success ? " success: " : " failed!") <<
			(success ? new_path.string() : "") << 
			std::endl << 
			std::endl;
		}
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}
