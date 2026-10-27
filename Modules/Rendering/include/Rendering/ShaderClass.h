#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include "UBO.h"
#include "SSBO.h"
#include <glm/gtc/type_ptr.hpp>

std::string get_shader_file_contents(std::filesystem::path& shader_path);

class ShaderProgram {
public:
	uint32_t id;
	ShaderProgram(
		std::filesystem::path& vertex_shader_path,
		std::filesystem::path& fragment_shader_path
	);
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram() = delete;
	~ShaderProgram();
	void use_shader() const;
	void delete_shader() const;
	void set_Uniform_Int(const char* uniform_name, const int value) const;
	void set_uniform_float(const char* uniform_name, const float value) const;
	void set_uniform_float_array(const char* uniform_name, const float* f_array, const int size) const;
	void set_Uniform_Mat4(const char* uniform_name, const glm::mat4& glm_matrix) const;
	void set_Uniform_Vec2(const char* uniform_name, const glm::vec2 glm_vec2) const;
	void set_Uniform_iVec2(const char* uniform_name, const glm::ivec2 glm_vec2) const;
	void set_Uniform_Vec3(const char* uniform_name, const glm::vec3 glm_vec3) const;
	void set_Uniform_Vec4(const char* uniform_name, const glm::vec4 glm_vec4) const;
	void set_ubo(const char* uniform_block_name, const UBO& ubo, const uint32_t index) const;
	void set_ssbo(const char* uniform_block_name, const SSBO& ssbo, const uint32_t index) const;
};