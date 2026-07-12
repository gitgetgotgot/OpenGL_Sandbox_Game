#include <Rendering/VAO.h>

VAO::VAO() {
	glGenVertexArrays(1, &this->id);
}

VAO::~VAO() {
	delete_VAO();
}

void VAO::link_Attribute(GLuint layout, GLuint numOfComps, GLenum type, GLsizeiptr stride, void* offset) const {
	glVertexAttribPointer(layout, numOfComps, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
}

void VAO::link_Attribute_with_Divisor(VBO& vbo, GLuint layout, GLuint numOfComps, GLenum type, GLsizeiptr stride, void* offset, GLuint divisor) const {
	vbo.bind_VBO();
	glVertexAttribPointer(layout, numOfComps, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	glVertexAttribDivisor(layout, divisor);
	vbo.unbind_VBO();
}

void VAO::bind_VAO() const {
	glBindVertexArray(this->id);
}

void VAO::unbind_VAO() const {
	glBindVertexArray(0);
}

void VAO::delete_VAO() const {
	glDeleteVertexArrays(1, &this->id);
}
