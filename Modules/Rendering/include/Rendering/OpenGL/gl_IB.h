#pragma once
#include "Rendering/RHI/IndexBuffer.h"

namespace Engine::Graphics {
	class GL_IB : public IndexBuffer {
	public:
		GL_IB();
		~GL_IB();
		void set_data(const void* data, size_t size, IndexBufUsage usage) override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void uninit() override;

		void bind() const;
		void unbind() const;
		uint32_t get_id() const { return id; }
	private:
		uint32_t id = 0;
	};
}