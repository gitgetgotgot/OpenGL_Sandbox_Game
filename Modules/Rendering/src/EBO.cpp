#include <Rendering/EBO.h>

EBO::EBO() {
	glGenBuffers(1, &this->id);
}

EBO::EBO(void* indices, GLsizeiptr size) {
	glGenBuffers(1, &this->id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

EBO::~EBO() {
	delete_EBO();
}

void EBO::set_data(void* indices, GLsizeiptr size, GLenum usage) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);
}

void EBO::update_data(void* indices, GLsizeiptr size, GLintptr offset) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, indices);
}

void EBO::bind_EBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void EBO::unbind_EBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void EBO::delete_EBO() const {
	glDeleteBuffers(1, &this->id);
}
