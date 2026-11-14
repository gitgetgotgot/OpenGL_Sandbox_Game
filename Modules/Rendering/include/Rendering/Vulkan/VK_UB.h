#pragma once
#include "Rendering/Interfaces/UniformBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine::Graphics {
	class VK_UB : public UniformBuffer {
	public:
		VK_UB(VkDevice device, VmaAllocator allocator, VkCommandPool command_pool, VkQueue graphics_queue);
		~VK_UB();
		void set_data(const void* data, size_t size, UniformBufUsage usage) const override;
		void update_data(const void* data, size_t size, size_t offset = 0) const override;
		void uninit() const override;
		void bind(uint32_t slot = 0) const override { bound_slot = slot; };
		void unbind() const override {};
		VkBuffer get_buffer() const;
		VkDescriptorBufferInfo get_descriptor_info() const;
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
		mutable UniformBufUsage buffer_usage;
		mutable uint32_t bound_slot = 0;
	};
}