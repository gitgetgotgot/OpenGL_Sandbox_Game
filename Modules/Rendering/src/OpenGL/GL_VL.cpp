#include "Rendering/OpenGL/gl_VL.h"
#include <glad/gl.h>

Engine::Graphics::GL_VL::GL_VL() {
	glCreateVertexArrays(1, &id);
}

Engine::Graphics::GL_VL::~GL_VL() {
	uninit();
}

void Engine::Graphics::GL_VL::add_attribute(uint32_t layout, VertexAttributeType type, size_t stride, size_t offset) {
	GLint size;
	GLenum gl_type;
	this->stride = stride;

	switch (type) {
		case VertexAttributeType::FLOAT:	  size = 1; gl_type = GL_FLOAT; break;
		case VertexAttributeType::FLOAT_VEC2: size = 2; gl_type = GL_FLOAT; break;
		case VertexAttributeType::FLOAT_VEC3: size = 3; gl_type = GL_FLOAT; break;
		case VertexAttributeType::FLOAT_VEC4: size = 4; gl_type = GL_FLOAT; break;

		case VertexAttributeType::UINT:		 size = 1; gl_type = GL_UNSIGNED_INT; break;
		case VertexAttributeType::UINT_VEC2: size = 2; gl_type = GL_UNSIGNED_INT; break;
		case VertexAttributeType::UINT_VEC3: size = 3; gl_type = GL_UNSIGNED_INT; break;
		case VertexAttributeType::UINT_VEC4: size = 4; gl_type = GL_UNSIGNED_INT; break;

		case VertexAttributeType::INT:		size = 1; gl_type = GL_INT; break;
		case VertexAttributeType::INT_VEC2: size = 2; gl_type = GL_INT; break;
		case VertexAttributeType::INT_VEC3: size = 3; gl_type = GL_INT; break;
		case VertexAttributeType::INT_VEC4: size = 4; gl_type = GL_INT; break;
	}

	glEnableVertexArrayAttrib(id, layout);

	if (gl_type == GL_FLOAT) {
		glVertexArrayAttribFormat(id, layout, size, gl_type, GL_FALSE, offset);
	}
	else {
		glVertexArrayAttribIFormat(id, layout, size, gl_type, offset);
	}

	glVertexArrayAttribBinding(id, layout, 0);
}

void Engine::Graphics::GL_VL::uninit() {
	if (id) {
		glDeleteVertexArrays(1, &id);
		id = 0;
	}
}

void Engine::Graphics::GL_VL::bind() const {
	glBindVertexArray(id);
}

void Engine::Graphics::GL_VL::unbind() const {
	glBindVertexArray(0);
}

void Engine::Graphics::GL_VL::bind_vertex_buffer(uint32_t vbo_id) const {
	glVertexArrayVertexBuffer(id, 0, vbo_id, 0, stride);
}

void Engine::Graphics::GL_VL::bind_index_buffer(uint32_t ibo_id) const {
	glVertexArrayElementBuffer(id, ibo_id);
}