#include <Rendering/UBO.h>

UBO::UBO() {
	glGenBuffers(1, &this->id);
}

UBO::~UBO() {
	delete_UBO();
}

void UBO::set_data(void* data, GLsizeiptr size, GLenum usage) const {
	glBindBuffer(GL_UNIFORM_BUFFER, this->id);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
}

void UBO::update_data(void* data, GLsizeiptr size, GLintptr offset) const {
	glBindBuffer(GL_UNIFORM_BUFFER, this->id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UBO::bind_UBO(uint32_t slot) const {
	glBindBuffer(GL_UNIFORM_BUFFER, this->id);
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, this->id);
}

void UBO::unbind_UBO() const {
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::delete_UBO() const {
	glDeleteBuffers(1, &this->id);
}
