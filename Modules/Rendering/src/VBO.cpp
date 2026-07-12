#include <Rendering/VBO.h>

VBO::VBO() {
	glGenBuffers(1, &this->id);
}

VBO::VBO(GLfloat* data, GLsizeiptr size) {
	glGenBuffers(1, &this->id);
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VBO::~VBO() {
	delete_VBO();
}

void VBO::set_data(void* data, GLsizeiptr size, GLenum usage) const {
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void VBO::update_data(void* data, GLsizeiptr size, GLintptr offset) const {
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void VBO::bind_VBO() const {
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VBO::unbind_VBO() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::delete_VBO() const {
	glDeleteBuffers(1, &this->id);
}
