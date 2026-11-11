#pragma once
#include <Rendering/Interfaces/Renderer.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine::Graphics {
	class Vulkan_Renderer : public Renderer {
	public:
		Vulkan_Renderer(VkSurfaceKHR surface, std::error_code& ec);
		~Vulkan_Renderer();
		void clear(float r, float g, float b) override;
		void end_frame() override;
	private:
		void create_instance(std::error_code& ec);
		void create_sufrace(std::error_code& ec);
		void pick_physical_device(std::error_code& ec);
		void create_logical_device(std::error_code& ec);
		void init_VMA(std::error_code& ec);
	public:
		void setViewport(float x, float y, float width, float height) override;
		void useDepthTest(bool use) override;
		void useDepthWrite(bool use) override;
		void useDepthFunc(DepthFunc func) override;
		void useScissorTest(bool use) override;
		void setScissorRect(float x, float y, float width, float height) override;

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
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue render_queue = VK_NULL_HANDLE;
		VmaAllocator allocator = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};
}