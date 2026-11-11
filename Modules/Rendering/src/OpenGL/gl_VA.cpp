#include "Rendering/OpenGL/gl_VL.h"
#include <glad/gl.h>

Engine::Graphics::gl_VL::gl_VL() {
	glGenVertexArrays(1, &id);
}

Engine::Graphics::gl_VL::~gl_VL() {
	uninit();
}

void Engine::Graphics::gl_VL::link_Attribute(uint32_t layout, uint32_t comps, ATTRIBUTE_TYPE type, size_t stride, size_t offset) const {
	if (type == ATTRIBUTE_TYPE::TYPE_FLOAT) {
		glVertexAttribPointer(layout, comps, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
	}
	else {
		glVertexAttribIPointer(layout, comps, GL_UNSIGNED_INT, stride, (const void*)offset);
	}
	glEnableVertexAttribArray(layout);
}

void Engine::Graphics::gl_VL::bind() const {
	glBindVertexArray(id);
}

void Engine::Graphics::gl_VL::unbind() const {
	glBindVertexArray(0);
}

void Engine::Graphics::gl_VL::uninit() const {
	if (id) glDeleteVertexArrays(1, &id);
}