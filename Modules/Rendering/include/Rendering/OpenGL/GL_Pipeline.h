#pragma once
#include "Rendering/Interfaces/Pipeline.h"

namespace Engine::Graphics {
	// OpenGL realization is blank (not actually used in render pipeline)
	class GL_Pipeline : public Pipeline {
	public:
		GL_Pipeline() = default;
		~GL_Pipeline() = default;
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
		) override {}
		void uninit() override {}
	};
}