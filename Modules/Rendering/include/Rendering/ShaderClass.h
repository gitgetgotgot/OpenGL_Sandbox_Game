#pragma once
#include <cstdint>
#include <filesystem>
#include <string>

namespace Engine::Graphics {
	class ShaderProgram {
	public:
		uint32_t id;
		ShaderProgram(
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path
		);
		~ShaderProgram();
		void use_shader() const;
		void delete_shader() const;
	private:
		std::string get_shader_file_contents(const std::filesystem::path& shader_path);
	};
}