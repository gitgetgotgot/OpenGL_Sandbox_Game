#pragma once
#include <cstdint>
#include <cstddef>

namespace Engine::Graphics {
	class StorageBuffer {
	public:
		enum StorageBufUsage : uint8_t { USAGE_STATIC, USAGE_DYNAMIC };
		virtual ~StorageBuffer() = default;
		virtual void set_data(const void* data, size_t size, StorageBufUsage usage) const = 0;
		virtual void update_data(const void* data, size_t size, size_t offset = 0) const = 0;
		virtual void bind(uint32_t slot = 0) const = 0;
		virtual void unbind() const = 0;
		virtual void uninit() const = 0;
	};
}