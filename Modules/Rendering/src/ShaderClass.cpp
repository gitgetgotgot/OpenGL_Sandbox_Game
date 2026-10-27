#include "Rendering/ShaderClass.h"
#include <glad/gl.h>
#include <fstream>
#include <iostream>

std::string get_shader_file_contents(std::filesystem::path& shader_path) {
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

ShaderProgram::ShaderProgram(
	std::filesystem::path& vertex_shader_path,
	std::filesystem::path& fragment_shader_path
) {
	std::string vertexContents = get_shader_file_contents(vertex_shader_path);
	std::string fragmentContetns = get_shader_file_contents(fragment_shader_path);
	const char* vertexSource = vertexContents.c_str();
	const char* fragmentSource = fragmentContetns.c_str();

	auto compileShader = [&](GLenum type, const char* src) {
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string info(length, '\0');
			glGetShaderInfoLog(shader, length, nullptr, info.data());

			std::cerr << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
				<< " SHADER ERROR:\n" << info << std::endl;
		}

		return shader;
		};

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	GLint success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (!success) {
		GLint length = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

		std::string info(length, '\0');
		glGetProgramInfoLog(id, length, nullptr, info.data());

		std::cerr << "PROGRAM LINK ERROR:\n" << info << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

ShaderProgram::~ShaderProgram() {
	delete_shader();
}

void ShaderProgram::use_shader() const {
	glUseProgram(id);
}

void ShaderProgram::delete_shader() const {
	glDeleteProgram(id);
}

void ShaderProgram::set_Uniform_Int(const char* uniform_name, const int value) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform1i(uniformID, value);
}

void ShaderProgram::set_uniform_float(const char* uniform_name, const float value) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform1f(uniformID, value);
}

void ShaderProgram::set_uniform_float_array(const char* uniform_name, const float* f_array, const int size) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform1fv(uniformID, size, f_array);
}

void ShaderProgram::set_Uniform_Mat4(const char* uniform_name, const glm::mat4& glm_matrix) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(glm_matrix));
}

void ShaderProgram::set_Uniform_Vec2(const char* uniform_name, const glm::vec2 glm_vec2) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform2fv(uniformID, 1, glm::value_ptr(glm_vec2));
}

void ShaderProgram::set_Uniform_iVec2(const char* uniform_name, const glm::ivec2 glm_vec2) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform2iv(uniformID, 1, glm::value_ptr(glm_vec2));
}

void ShaderProgram::set_Uniform_Vec3(const char* uniform_name, const glm::vec3 glm_vec3) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform3fv(uniformID, 1, glm::value_ptr(glm_vec3));
}

void ShaderProgram::set_Uniform_Vec4(const char* uniform_name, const glm::vec4 glm_vec4) const {
	GLuint uniformID = glGetUniformLocation(id, uniform_name);
	glUniform4fv(uniformID, 1, glm::value_ptr(glm_vec4));
}

void ShaderProgram::set_ubo(const char* uniform_block_name, const UBO& ubo, const uint32_t index) const {
	GLuint blockIndex = glGetUniformBlockIndex(id, uniform_block_name);
	glUniformBlockBinding(id, blockIndex, index);
	glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo.id);
}

void ShaderProgram::set_ssbo(const char* uniform_block_name, const SSBO& ssbo, const uint32_t index) const {
	GLuint blockIndex = glGetProgramResourceIndex(id, GL_SHADER_STORAGE_BLOCK, uniform_block_name);
	glShaderStorageBlockBinding(id, blockIndex, index);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo.id);
}
