#pragma once
#include "Rendering/Interfaces/Pipeline.h"

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

		void bind() const;
	private:
		// pipeline for OpenGL uses only shader and vertex layuot
		const ShaderHandle* shader;
		const VertexLayout* vl;
	};
}