#pragma once

#include <glad/glad.h>
#include <cstdint>

class UBO {
public:
	GLuint id;
	UBO();
	~UBO();
	void set_data(void* data, GLsizeiptr size, GLenum usage) const;
	void update_data(void* data, GLsizeiptr size, GLintptr offset = 0) const;
	void bind_UBO(uint32_t slot = 0) const;
	void unbind_UBO() const;
	void delete_UBO() const;
};