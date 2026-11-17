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
		void bind_uniform_buffer(const UniformBuffer& ub, uint32_t binding = 0, uint32_t set = 0) override;
		void bind_storage_buffer(const StorageBuffer& sb, uint32_t binding = 0, uint32_t set = 0) override;
		void bind_sampler_2D(const Texture& tex, uint32_t binding = 0, uint32_t set = 0) override;
		void bind_sampler_2D_array(const TextureArray& tex_arr, uint32_t binding = 0, uint32_t index, uint32_t set = 0) override;

		void bind() const;

		// pipeline for OpenGL uses only shader and vertex layuot
		const ShaderHandle* shader;
		const VertexLayout* vl;
	};
}