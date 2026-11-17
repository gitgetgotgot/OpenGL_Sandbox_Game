#pragma once
#include "Rendering/RHI/TextureArray.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine::Graphics {
	class VK_TextureArray : public TextureArray {
	public:
		VK_TextureArray(VkDevice device, VmaAllocator allocator, VkCommandPool command_pool, VkQueue graphics_queue);
		~VK_TextureArray();

		void setup_texture_array(
			uint32_t width, uint32_t height, uint32_t depth, FilterMode filter, WrapMode wrap,
			uint8_t channels, ResizeStrategy resize_str, std::error_code& ec
		) override;
		void add_2D_texture(const char* fileName, std::error_code& ec) override;
		void add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels, std::error_code& ec) override;

		VkDescriptorImageInfo get_descriptor_info() const;
	private:
		void create_tex_array(
			VkImage& new_image, VmaAllocation& new_allocation, VkImageView& new_image_view, uint32_t new_depth, std::error_code& ec
		);
		void resize_array(std::error_code& ec);
		void create_and_begin_temp_cmd_buffer(VkCommandBuffer& cmd_buf) const;
		void submit_and_destroy_temp_cmd_buffer(VkCommandBuffer& cmd_buf) const;
		void change_image_layer_layouts(
			VkImage& target_image, VkCommandBuffer& cmd_buf,
			VkImageLayout old_layout, VkImageLayout new_layout,
			uint32_t base_layer, uint32_t layer_count
		) const;
		void mark_as_dirty(); // to rebind resized array later for different pipelines
		
		VkDevice device = VK_NULL_HANDLE;
		VmaAllocator allocator = VK_NULL_HANDLE;
		VkCommandPool command_pool = VK_NULL_HANDLE;
		VkQueue graphics_queue = VK_NULL_HANDLE;

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkImageView image_view = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
	};
}