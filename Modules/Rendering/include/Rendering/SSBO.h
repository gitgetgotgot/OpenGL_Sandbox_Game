#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class SSBO {
public:
	GLuint id;
	SSBO();
	~SSBO();
	void set_data(void* data, GLsizeiptr size, GLenum usage) const;
	void update_data(void* data, GLsizeiptr size, GLintptr offset = 0) const;
	void set_persistent_storage_data(void* data, GLsizeiptr size) const;
	void bind_SSBO(uint32_t slot = 0) const;
	void unbind_SSBO() const;
	void delete_SSBO() const;
};