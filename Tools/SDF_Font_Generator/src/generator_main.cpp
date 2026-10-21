#include "FontGenerator.h"
#include <iostream>

int main() {
	if (!std::filesystem::exists(input_fonts_folder)) {
		std::cout << "<InputFolder> with fonts not found in executable folder\n";
		return 0;
	}
	if (!std::filesystem::exists(output_fonts_folder)) {
		std::filesystem::create_directory(output_fonts_folder);
		std::cout << "<OutputFolder> created in executable folder\n";
	}
	for (auto& entry : std::filesystem::directory_iterator(input_fonts_folder)) {
		std::filesystem::path file_path = entry.path();
		if (std::filesystem::is_regular_file(file_path) && file_path.extension() == ".ttf") {
			std::string filename = file_path.filename().string();
			std::string sdf_name = filename.substr(0, filename.find_last_of('.')) + "_SDF";
			std::cout << "Found font <" << filename << ">\n";
			std::cout << "	Font generation has started...\n";
			if (FontGenerator::generate_truetype_sdf_font(file_path, true)) {
				std::cout << "	Font <" << sdf_name << "> was generated in folder <" << sdf_name << ">\n";
			}
			std::cout << std::endl;
		}
	}
	return 1;
}