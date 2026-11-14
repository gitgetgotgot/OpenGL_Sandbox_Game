#include "Rendering/OpenGL/gl_VB.h"
#include <glad/gl.h>

Engine::Graphics::GL_VB::GL_VB() {
	glGenBuffers(1, &id);
}

Engine::Graphics::GL_VB::~GL_VB() {
	uninit();
}

void Engine::Graphics::GL_VB::set_data(const void* data, size_t size, VertexBufUsage usage) const {
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage == VertexBufUsage::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::GL_VB::update_data(const void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void Engine::Graphics::GL_VB::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void Engine::Graphics::GL_VB::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Engine::Graphics::GL_VB::uninit() {
	if (id) glDeleteBuffers(1, &id);
}