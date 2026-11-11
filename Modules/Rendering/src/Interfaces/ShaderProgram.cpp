#include "Rendering/Interfaces/ShaderProgram.h"
#include <fstream>
#include <iostream>

std::string Engine::Graphics::ShaderProgram::get_shader_file_contents(const std::filesystem::path& shader_path) const {
	std::ifstream in(shader_path, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw std::runtime_error("Can't load shader <" + shader_path.string() + "> !!!");
}