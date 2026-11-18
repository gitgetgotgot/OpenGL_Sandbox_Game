#pragma once
#include "Rendering/RHI/Pipeline.h"

namespace Engine::Graphics {
	class GL_Pipeline : public Pipeline {
	public:
		GL_Pipeline() = default;
		~GL_Pipeline();
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
		void bind_uniform_buffer(UniformBuffer& ub, uint32_t binding = 0, uint32_t set = 0) const override;
		void bind_storage_buffer(StorageBuffer& sb, uint32_t binding = 0, uint32_t set = 0) const override;
		void bind_sampler_2D(Texture& tex, uint32_t binding = 0, uint32_t set = 0) const override;
		void bind_sampler_2D_array(TextureArray& tex_arr, uint32_t binding = 0, uint32_t index = 0, uint32_t set = 0) const override;

		void bind() const;

		// pipeline for OpenGL uses only shader and vertex layuot
		const ShaderHandle* shader;
		const VertexLayout* vl;
	};
}