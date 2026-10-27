#include "Rendering/VAO.h"
#include <glad/gl.h>

Engine::Graphics::VAO::VAO() {
	glGenVertexArrays(1, &id);
}

Engine::Graphics::VAO::~VAO() {
	delete_VAO();
}

void Engine::Graphics::VAO::link_Attribute(uint32_t layout, uint32_t comps, VAO_ATTRIBUTE_TYPE type, size_t stride, size_t offset) const {
	if (type == VAO_ATTRIBUTE_TYPE::TYPE_FLOAT) {
		glVertexAttribPointer(layout, comps, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
	}
	else {
		glVertexAttribIPointer(layout, comps, GL_UNSIGNED_INT, stride, (const void*)offset);
	}
	glEnableVertexAttribArray(layout);
}

void Engine::Graphics::VAO::bind_VAO() const {
	glBindVertexArray(id);
}

void Engine::Graphics::VAO::unbind_VAO() const {
	glBindVertexArray(0);
}

void Engine::Graphics::VAO::delete_VAO() const {
	if (id) glDeleteVertexArrays(1, &id);
}