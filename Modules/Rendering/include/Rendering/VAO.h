#pragma once
#include "VBO.h"

class VAO {
public:
	GLuint id;
	VAO();
	~VAO();
	void link_Attribute(GLuint layout, GLuint numOfComps, GLenum type, GLsizeiptr stride, void* offset) const;
	void link_Attribute_with_Divisor(VBO& vbo, GLuint layout, GLuint numOfComps, GLenum type, GLsizeiptr stride, void* offset, GLuint divisor) const;
	void bind_VAO() const;
	void unbind_VAO() const;
	void delete_VAO() const;
};