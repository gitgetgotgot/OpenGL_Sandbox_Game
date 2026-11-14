#pragma once
#include "Rendering/Interfaces/IndexBuffer.h"

namespace Engine::Graphics {
	class GL_IB : public IndexBuffer {
	public:
		GL_IB();
		~GL_IB();
		void set_data(const void* data, size_t size, IndexBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void bind() const override;
		void unbind() const override;
		void uninit() override;
		uint32_t id = 0;
	};
}