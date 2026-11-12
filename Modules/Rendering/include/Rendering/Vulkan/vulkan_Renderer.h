#pragma once
#include <Rendering/Interfaces/Renderer.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vector>

namespace Engine::Graphics {
	class Vulkan_Renderer : public Renderer {
	public:
		Vulkan_Renderer(VkInstance instance, VkSurfaceKHR surface, std::error_code& ec);
		~Vulkan_Renderer();
	private:
		void pick_physical_device(std::error_code& ec);
		bool is_device_suitable(VkPhysicalDevice dev);
		void create_logical_device(std::error_code& ec);
		void init_VMA(std::error_code& ec);

		void create_Swap_Chain(std::error_code& ec);
		void recreate_Swap_Chain(
			uint32_t new_extent_width, uint32_t new_extent_height, PresentMode mode, std::error_code& ec
		);
		void create_render_pass(std::error_code& ec);
		void create_frame_buffers(std::error_code& ec);
		void create_command_pool_and_buffers(std::error_code& ec);
		void create_semaphores_and_fences(std::error_code& ec);
	public:
		void start_frame(std::error_code& ec) override;
		void clear(float r, float g, float b) override;
		void present_frame(std::error_code& ec) override;
		void on_window_resize(uint32_t window_width, uint32_t window_height, PresentMode mode) override;
		void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void use_depth_test(bool use) override;
		void use_depth_write(bool use) override;
		void use_depth_func(DepthFunc func) override;
		void use_scissor_test(bool use) override;
		void set_scissor_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		std::unique_ptr<ShaderProgram> create_ShaderPr(
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path,
			std::error_code& ec
		) const override;
		std::unique_ptr<VertexLayout> create_VertexLayout(std::error_code& ec) const override;
		std::unique_ptr<VertexBuffer> create_VertexBuffer(std::error_code& ec) const override;
		std::unique_ptr<IndexBuffer> create_IndexBuffer(std::error_code& ec) const override;
		std::unique_ptr<UniformBuffer> create_UniformBuffer(std::error_code& ec) const override;
		std::unique_ptr<StorageBuffer> create_StorageBuffer(std::error_code& ec) const override;
		std::unique_ptr<Texture> create_Texture(std::error_code& ec) const override;
		std::unique_ptr<TextureArray> create_TextureArray(std::error_code& ec) const override;

		void renderData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb
		) override;
		void renderIndexedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t baseIndex = 0
		) override;
		void renderIndexedDataVAOSpecific(
			const VertexLayout& va,
			uint32_t indexCount
		) override;
		void renderInstancedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount
		) override;
		void renderBasedInstancedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t baseInstance
		) override;
	private:
		struct VulkanSwapChain {
			VkSwapchainKHR chain = VK_NULL_HANDLE;
			uint32_t image_count;
			VkFormat image_format;
			VkColorSpaceKHR image_color_space;
			VkExtent2D extent;
			std::vector<VkImage> images;
			std::vector<VkImageView> image_views;
			std::vector<VkFramebuffer> framebuffers;
			bool has_mailbox_present_mode;
		};
	private:
		// main handles
		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkSurfaceTransformFlagBitsKHR surface_transform;

		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		uint32_t graphics_queue_family_index = 0;

		VkQueue graphics_queue = VK_NULL_HANDLE;
		VkQueue present_queue = VK_NULL_HANDLE;

		VmaAllocator allocator = VK_NULL_HANDLE;
		
		// Swap Chain
		VulkanSwapChain swap_chain;
		
		// Render Pass
		VkRenderPass render_pass = VK_NULL_HANDLE;

		// Command pool & buffers
		VkCommandPool command_pool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> command_buffers;

		// Semaphores(GPU <-> GPU) and Fences(CPU <-> GPU)
		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		std::vector<VkSemaphore> image_available_semaphores;
		std::vector<VkSemaphore> render_finished_semaphores;
		std::vector<VkFence> in_flight_fences;

		// current image and frame index
		uint32_t current_image_index;
		uint32_t current_frame_index;

		//current present mode
		PresentMode current_present_mode;
	};
}