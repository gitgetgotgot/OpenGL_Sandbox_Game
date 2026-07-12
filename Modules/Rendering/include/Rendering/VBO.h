#pragma once

#include <glad/glad.h>

class VBO {
public:
	GLuint id;
	VBO();
	VBO(GLfloat* data, GLsizeiptr size);
	~VBO();
	void set_data(void* data, GLsizeiptr size, GLenum usage) const;
	void update_data(void* data, GLsizeiptr size, GLintptr offset = 0) const;
	void bind_VBO() const;
	void unbind_VBO() const;
	void delete_VBO() const;
};