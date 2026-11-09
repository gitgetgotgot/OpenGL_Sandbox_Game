#include "Rendering/EBO.h"
#include <glad/gl.h>

Engine::Graphics::EBO::EBO() {
	glGenBuffers(1, &id);
}

Engine::Graphics::EBO::EBO(const void* data, size_t size) {
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

Engine::Graphics::EBO::~EBO() {
	delete_EBO();
}

void Engine::Graphics::EBO::set_data(const void* data, size_t size, EBO_USAGE usage) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage == EBO_USAGE::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::EBO::update_data(const void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void Engine::Graphics::EBO::bind_EBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Engine::Graphics::EBO::unbind_EBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Engine::Graphics::EBO::delete_EBO() const {
	if (id) glDeleteBuffers(1, &id);
}
