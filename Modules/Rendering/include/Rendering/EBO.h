#pragma once

#include <glad/glad.h>

class EBO {
public:
	GLuint id;
	EBO();
	EBO(void* indices, GLsizeiptr size);
	~EBO();
	void set_data(void* indices, GLsizeiptr size, GLenum usage) const;
	void update_data(void* indices, GLsizeiptr size, GLintptr offset = 0) const;
	void bind_EBO() const;
	void unbind_EBO() const;
	void delete_EBO() const;
};