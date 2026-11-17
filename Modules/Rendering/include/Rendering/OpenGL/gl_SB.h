#pragma once
#include "Rendering/RHI/StorageBuffer.h"

namespace Engine::Graphics {
	class GL_SB : public StorageBuffer {
	public:
		GL_SB();
		~GL_SB();
		void set_data(const void* data, size_t size, StorageBufUsage usage) override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void uninit() override;

		void bind(uint32_t slot = 0) const;
		void unbind() const;
	private:
		uint32_t id = 0;
	};
}