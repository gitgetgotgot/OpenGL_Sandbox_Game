#pragma once
#include "Rendering/Interfaces/StorageBuffer.h"

namespace Engine::Graphics {
	class gl_SB : public StorageBuffer {
	public:
		gl_SB();
		~gl_SB();
		void set_data(const void* data, size_t size, StorageBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void bind(uint32_t slot = 0) const override;
		void unbind() const override;
		void uninit() const override;
		uint32_t id = 0;
	};
}