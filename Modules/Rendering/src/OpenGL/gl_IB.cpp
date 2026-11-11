#include "Rendering/OpenGL/gl_IB.h"
#include <glad/gl.h>

Engine::Graphics::gl_IB::gl_IB() {
	glGenBuffers(1, &id);
}

Engine::Graphics::gl_IB::~gl_IB() {
	uninit();
}

void Engine::Graphics::gl_IB::set_data(const void* data, size_t size, IndexBufUsage usage) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage == IndexBufUsage::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Engine::Graphics::gl_IB::update_data(const void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void Engine::Graphics::gl_IB::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Engine::Graphics::gl_IB::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void Engine::Graphics::gl_IB::uninit() const {
	if (id) glDeleteBuffers(1, &id);
}
