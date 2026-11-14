#pragma once
#include "Rendering/Interfaces/UniformBuffer.h"

namespace Engine::Graphics {
	class GL_UB : public UniformBuffer {
	public:
		GL_UB();
		~GL_UB();
		void set_data(const void* data, size_t size, UniformBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void bind(uint32_t slot = 0) const override;
		void unbind() const override;
		void uninit() override;
	private:
		uint32_t id = 0;
	};
}