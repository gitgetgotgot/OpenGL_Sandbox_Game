#include "Rendering/OpenGL/gl_ShaderPr.h"
#include <glad/gl.h>
#include <iostream>

Engine::Graphics::gl_ShaderPr::gl_ShaderPr(
	const std::filesystem::path& vertex_shader_path,
	const std::filesystem::path& fragment_shader_path
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

Engine::Graphics::gl_ShaderPr::~gl_ShaderPr() {
	delete_shader();
}

void Engine::Graphics::gl_ShaderPr::use_shader() const {
	glUseProgram(id);
}

void Engine::Graphics::gl_ShaderPr::delete_shader() const {
	glDeleteProgram(id);
}