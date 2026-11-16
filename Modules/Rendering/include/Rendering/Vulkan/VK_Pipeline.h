#pragma once
#include "Rendering/Interfaces/Pipeline.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine::Graphics {
	class VK_Pipeline : public Pipeline {
	public:
		VK_Pipeline(VkDevice device, VkRenderPass render_pass);
		~VK_Pipeline();
		void init(
			const ShaderHandle& shader,
			const VertexLayout& layout,
			PipelinePrimitives p,
			PipelineScissor s,
			PipelineViewport v,
			PipelineBlendMode bm,
			PipelineDepthMode dm,
			PipelineCullMode cm,
			std::error_code& ec
		) override;
		void uninit() override;
		void bind_uniform_buffer(const UniformBuffer& ub, uint32_t binding = 0, uint32_t set = 0) override;
		void bind_storage_buffer(const StorageBuffer& sb, uint32_t binding = 0, uint32_t set = 0) override;
		void bind_sampler_2D(const Texture& tex, uint32_t binding = 0, uint32_t set = 0) override;
		void bind_sampler_2D_array(const TextureArray& tex_arr, uint32_t binding = 0, uint32_t set = 0) override;

		VkPipeline get_handle() const { return pipeline; }
		VkPipelineLayout get_layout_handle() const { return pipeline_layout; }
		const std::vector<VkDescriptorSet>& get_descriptor_sets() const { return descriptor_sets; }
	private:
		VkDevice device = VK_NULL_HANDLE;
		VkRenderPass render_pass = VK_NULL_HANDLE;

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

		// Descriptors for shader resources (textures, uniform/storage buffers)
		VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

		//std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
		std::vector<VkDescriptorSet> descriptor_sets;
	};
}