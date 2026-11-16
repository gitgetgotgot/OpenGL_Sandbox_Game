#pragma once
#include "Rendering/Interfaces/VertexLayout.h"

namespace Engine::Graphics {
	class GL_VL : public VertexLayout {
	public:
		GL_VL();
		~GL_VL();
		void add_attribute(uint32_t layout, VertexAttributeType type, size_t stride, size_t offset) override;
		void bind() const override;
		void unbind() const override;
		void uninit() override;
	private:
		uint32_t id = 0;
	};
}