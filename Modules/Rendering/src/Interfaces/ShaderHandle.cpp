#include "Rendering/Interfaces/ShaderHandle.h"
#include <Base/EngineError.h>
#include <fstream>
#include <iostream>
#include <cstddef>

std::string Engine::Graphics::ShaderHandle::read_file_string(
	const std::filesystem::path& shader_path,
	std::error_code& ec
) const {
	std::ifstream in(shader_path, std::ios::binary);
	if (!in.is_open()) {
		ec = Engine::Errors::EngineError::ShaderFileLoadError;
		return {};
	}
	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();
	return contents;
}

std::vector<uint32_t> Engine::Graphics::ShaderHandle::read_file_uint32_t(
	const std::filesystem::path& shader_path,
	std::error_code& ec
) const {
	std::ifstream in(shader_path, std::ios::binary);
	if (!in.is_open()) {
		ec = Engine::Errors::EngineError::ShaderFileLoadError;
		return {};
	}
	in.seekg(0, std::ios::end);
	size_t file_size = static_cast<size_t>(in.tellg());
	if (file_size % sizeof(uint32_t) != 0) {
		ec = Engine::Errors::EngineError::ShaderFileLoadError;
		return {};
	}
	std::vector<uint32_t> contents(file_size / sizeof(uint32_t));
	in.seekg(0, std::ios::beg);
	in.read(reinterpret_cast<char*>(contents.data()), file_size);
	in.close();
	return contents;
}