#include "Rendering/ShaderClass.h"
#include <glad/gl.h>
#include <fstream>
#include <iostream>

Engine::Graphics::ShaderProgram::ShaderProgram(
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

Engine::Graphics::ShaderProgram::~ShaderProgram() {
	delete_shader();
}

void Engine::Graphics::ShaderProgram::use_shader() const {
	glUseProgram(id);
}

void Engine::Graphics::ShaderProgram::delete_shader() const {
	glDeleteProgram(id);
}

std::string Engine::Graphics::ShaderProgram::get_shader_file_contents(std::filesystem::path& shader_path) {
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