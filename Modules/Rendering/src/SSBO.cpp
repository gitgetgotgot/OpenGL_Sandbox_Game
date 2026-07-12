#include <Rendering/SSBO.h>

SSBO::SSBO() {
	glGenBuffers(1, &this->id);
}

SSBO::~SSBO() {
	delete_SSBO();
}

void SSBO::set_data(void* data, GLsizeiptr size, GLenum usage) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
}

void SSBO::update_data(void* data, GLsizeiptr size, GLintptr offset) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void SSBO::set_persistent_storage_data(void* data, GLsizeiptr size) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void SSBO::bind_SSBO(uint32_t slot) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, this->id);
}

void SSBO::unbind_SSBO() const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::delete_SSBO() const {
	glDeleteBuffers(1, &this->id);
}

