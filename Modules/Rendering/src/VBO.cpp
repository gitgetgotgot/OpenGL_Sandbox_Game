#include "Rendering/VBO.h"
#include <glad/gl.h>

Engine::Graphics::VBO::VBO() {
	glGenBuffers(1, &id);
}

Engine::Graphics::VBO::VBO(void* data, size_t size) {
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

Engine::Graphics::VBO::~VBO() {
	delete_VBO();
}

void Engine::Graphics::VBO::set_data(void* data, size_t size, VBO_USAGE usage) const {
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage == VBO_USAGE::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::VBO::update_data(void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void Engine::Graphics::VBO::bind_VBO() const {
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void Engine::Graphics::VBO::unbind_VBO() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Engine::Graphics::VBO::delete_VBO() const {
	if (id) glDeleteBuffers(1, &id);
}