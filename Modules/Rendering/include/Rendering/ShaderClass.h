#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include "UBO.h"
#include "SSBO.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine::Graphics {
	class ShaderProgram {
	public:
		uint32_t id;
		ShaderProgram(
			std::filesystem::path& vertex_shader_path,
			std::filesystem::path& fragment_shader_path
		);
		~ShaderProgram();
		void use_shader() const;
		void delete_shader() const;
	private:
		std::string get_shader_file_contents(std::filesystem::path& shader_path);
	};
}