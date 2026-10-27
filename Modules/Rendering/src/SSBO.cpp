#include "Rendering/SSBO.h"
#include <glad/gl.h>

SSBO::SSBO() {
	glGenBuffers(1, &id);
}

SSBO::~SSBO() {
	delete_SSBO();
}

void SSBO::set_data(void* data, size_t size, SSBO_USAGE usage) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage == SSBO_USAGE::USAGE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void SSBO::update_data(void* data, size_t size, size_t offset) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void SSBO::set_persistent_storage_data(void* data, size_t size) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void SSBO::bind_SSBO(uint32_t slot) const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, id);
}

void SSBO::unbind_SSBO() const {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::delete_SSBO() const {
	if (id) glDeleteBuffers(1, &id);
}

