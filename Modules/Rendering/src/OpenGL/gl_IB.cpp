#include "Rendering/OpenGL/gl_IB.h"
#include <glad/gl.h>

Engine::Graphics::GL_IB::GL_IB() {
	glGenBuffers(1, &id);
}

Engine::Graphics::GL_IB::~GL_IB() {
	uninit();
}

void Engine::Graphics::GL_IB::set_data(const void* data, size_t size, IndexBufUsage usage) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage == IndexBufUsage::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::GL_IB::update_data(const void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void Engine::Graphics::GL_IB::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Engine::Graphics::GL_IB::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Engine::Graphics::GL_IB::uninit() {
	if (id) glDeleteBuffers(1, &id);
}
