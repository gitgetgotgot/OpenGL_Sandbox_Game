#pragma once
#include "Rendering/RHI/VertexLayout.h"

namespace Engine::Graphics {
	class GL_VL : public VertexLayout {
	public:
		GL_VL();
		~GL_VL();
		void add_attribute(uint32_t layout, VertexAttributeType type, size_t stride, size_t offset) override;
		void uninit() override;

		void bind() const;
		void unbind() const;
		void bind_vertex_buffer(uint32_t vbo_id) const;
		void bind_index_buffer(uint32_t ibo_id) const;
	private:
		uint32_t id = 0;
		size_t stride = 0;
	};
}