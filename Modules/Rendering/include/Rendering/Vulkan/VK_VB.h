#pragma once
#include "Rendering/Interfaces/VertexBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine::Graphics {
	class VK_VB : public VertexBuffer {
	public:
		VK_VB(VkDevice device, VmaAllocator allocator, VkCommandPool command_pool, VkQueue graphics_queue);
		~VK_VB();
		void set_data(const void* data, size_t size, VertexBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void uninit() const override;
		void bind() const override {};
		void unbind() const override {};
		VkBuffer get_buffer() const;
	private:
		void send_data_to_static_buffer(const void* data, size_t size, size_t offset) const;

		VkDevice device = VK_NULL_HANDLE;
		VmaAllocator allocator = VK_NULL_HANDLE;
		VkCommandPool command_pool = VK_NULL_HANDLE;
		VkQueue graphics_queue = VK_NULL_HANDLE;

		mutable VkBuffer buffer = VK_NULL_HANDLE;
		mutable VmaAllocation allocation = VK_NULL_HANDLE;
		mutable void* mapped_data = nullptr;
		mutable size_t buffer_size = 0;
		mutable VertexBufUsage buffer_usage;
	};
}