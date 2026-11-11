#include "Rendering/OpenGL/gl_UB.h"
#include <glad/gl.h>

Engine::Graphics::gl_UB::gl_UB() {
	glGenBuffers(1, &id);
}

Engine::Graphics::gl_UB::~gl_UB() {
	uninit();
}

void Engine::Graphics::gl_UB::set_data(const void* data, size_t size, UniformBufUsage usage) const {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage == UniformBufUsage::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::gl_UB::update_data(const void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void Engine::Graphics::gl_UB::bind(uint32_t slot) const {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, id);
}

void Engine::Graphics::gl_UB::unbind() const {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Engine::Graphics::gl_UB::uninit() const {
	if (id) glDeleteBuffers(1, &id);
}