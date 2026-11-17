#pragma once
#include "Rendering/RHI/VertexBuffer.h"

namespace Engine::Graphics {
	class GL_VB : public VertexBuffer {
	public:
		GL_VB();
		~GL_VB();
		void set_data(const void* data, size_t size, VertexBufUsage usage) override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void uninit() override;

		void bind() const;
		void unbind() const;
		uint32_t get_id() const { return id; }
	private:
		uint32_t id = 0;
	};
}