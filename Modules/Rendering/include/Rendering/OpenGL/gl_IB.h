#pragma once
#include "Rendering/Interfaces/IndexBuffer.h"

namespace Engine::Graphics {
	class gl_IB : public IndexBuffer {
	public:
		gl_IB();
		~gl_IB();
		void set_data(const void* data, size_t size, IndexBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void bind() const override;
		void unbind() const override;
		void uninit() const override;
		uint32_t id = 0;
	};
}