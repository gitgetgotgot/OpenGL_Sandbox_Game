#include "Rendering/OpenGL/gl_SB.h"
#include <glad/gl.h>

Engine::Graphics::gl_SB::gl_SB() {
	glGenBuffers(1, &id);
}

Engine::Graphics::gl_SB::~gl_SB() {
	uninit();
}

void Engine::Graphics::gl_SB::set_data(const void* data, size_t size, StorageBufUsage usage) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage == StorageBufUsage::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::gl_SB::update_data(const void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void Engine::Graphics::gl_SB::bind(uint32_t slot) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, id);
}

void Engine::Graphics::gl_SB::unbind() const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Engine::Graphics::gl_SB::uninit() const {
	if (id) glDeleteBuffers(1, &id);
}