#include <iostream>
#include <filesystem>
#include <Utility/DataParser.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "DataCompiler usage: DataCompiler <input_root> <output_root>\n";
        return 1;
    }

    std::filesystem::path input_root = argv[1];
    std::filesystem::path output_root = argv[2];

    if (!std::filesystem::exists(input_root)) {
        std::cerr << "Input root does not exist: " << input_root << "\n";
        return 1;
    }
    if (!std::filesystem::exists(output_root)) {
        std::cerr << "Output root does not exist: " << output_root << "\n";
        return 1;
    }

    return 0;
}