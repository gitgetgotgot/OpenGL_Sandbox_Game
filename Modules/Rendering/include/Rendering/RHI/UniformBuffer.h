#pragma once
#include <cstdint>
#include <cstddef>

namespace Engine::Graphics {
	class UniformBuffer {
	public:
		enum UniformBufUsage : uint8_t { USAGE_STATIC, USAGE_DYNAMIC };
		virtual ~UniformBuffer() = default;
		virtual void set_data(const void* data, size_t size, UniformBufUsage usage) = 0;
		virtual void update_data(const void* data, size_t size, size_t offset = 0) const = 0;
		virtual void uninit() = 0;
	};
}