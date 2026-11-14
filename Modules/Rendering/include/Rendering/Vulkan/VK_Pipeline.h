#pragma once
#include "Rendering/Interfaces/Pipeline.h"
#include <vulkan/vulkan.h>

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

		VkPipeline get_handle() const;
	private:
		VkDevice device = VK_NULL_HANDLE;
		VkRenderPass render_pass = VK_NULL_HANDLE;

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
	};
}