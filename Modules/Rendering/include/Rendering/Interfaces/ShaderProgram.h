#pragma once
#include <cstdint>
#include <filesystem>
#include <string>

namespace Engine::Graphics {
	class ShaderProgram {
	public:
		virtual ~ShaderProgram() = default;
		virtual void use_shader() const = 0;
		virtual void delete_shader() const = 0;
	protected:
		std::string get_shader_file_contents(const std::filesystem::path& shader_path) const;
	};
}