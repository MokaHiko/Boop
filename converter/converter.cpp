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

		if (p.path().extension() == ".png") {
			auto new_path = p.path();
			new_path.replace_extension(".bp");
			success = boop::convert_image(p.path(), new_path);
		}

		if (p.path().extension() == ".obj") {
			auto new_path = p.path();
			new_path.replace_extension(".bp");
			success = boop::convert_mesh(p.path(), new_path);
		}

		if (p.path().extension() == ".fbx") {
			auto new_path = p.path();
			new_path.replace_extension(".bp");
			success = boop::convert_model(p.path(), new_path);
		}

		// Check if file format is valid
		if (success.has_value()) {
			std::cout <<
			(success ? " success!" : " failed!") <<
			std::endl << 
			std::endl;
		}
	}
}
