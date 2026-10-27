#include "Rendering/EBO.h"
#include <glad/gl.h>

EBO::EBO() {
	glGenBuffers(1, &id);
}

EBO::EBO(void* indices, size_t size) {
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

EBO::~EBO() {
	delete_EBO();
}

void EBO::set_data(void* indices, size_t size, EBO_USAGE usage) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage == EBO_USAGE::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void EBO::update_data(void* indices, size_t size, size_t offset) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, indices);
}

void EBO::bind_EBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::unbind_EBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::delete_EBO() const {
	if (id) glDeleteBuffers(1, &id);
}
