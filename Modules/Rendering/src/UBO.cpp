#include "Rendering/UBO.h"
#include <glad/gl.h>

UBO::UBO() {
	glGenBuffers(1, &id);
}

UBO::~UBO() {
	delete_UBO();
}

void UBO::set_data(void* data, size_t size, UBO_USAGE usage) const {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage == UBO_USAGE::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void UBO::update_data(void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UBO::bind_UBO(uint32_t slot) const {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, id);
}

void UBO::unbind_UBO() const {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::delete_UBO() const {
	if (id) glDeleteBuffers(1, &id);
}
