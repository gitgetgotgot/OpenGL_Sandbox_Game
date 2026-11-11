#pragma once
#include "Rendering/Interfaces/VertexLayout.h"

namespace Engine::Graphics {
	class gl_VL : public VertexLayout {
	public:
		gl_VL();
		~gl_VL();
		void link_Attribute(uint32_t layout, uint32_t comps, ATTRIBUTE_TYPE type, size_t stride, size_t offset) const override;
		void bind() const override;
		void unbind() const override;
		void uninit() const override;
		uint32_t id = 0;
	};
}