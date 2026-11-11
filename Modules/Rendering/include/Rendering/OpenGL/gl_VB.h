#pragma once
#include "Rendering/Interfaces/VertexBuffer.h"

namespace Engine::Graphics {
	class gl_VB : public VertexBuffer {
	public:
		gl_VB();
		~gl_VB();
		void set_data(const void* data, size_t size, VertexBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void bind() const override;
		void unbind() const override;
		void uninit() const override;
		uint32_t id = 0;
	};
}