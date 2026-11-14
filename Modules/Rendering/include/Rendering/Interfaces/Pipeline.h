#pragma once
#include "Rendering/Interfaces/ShaderHandle.h"
#include "Rendering/Interfaces/VertexLayout.h"
#include <cstdint>
#include <cstddef>
#include <system_error>

namespace Engine::Graphics {
	class Pipeline {
	public:
		enum class PipelinePrimitives : uint8_t { Point, Line, Triangle };
		enum class PipelineScissor : uint8_t { Static, Dynamic };
		enum class PipelineViewport : uint8_t { Static, Dynamic };
		enum class PipelineBlendMode : uint8_t { None, Alpha, Additive };
		enum class PipelineDepthMode : uint8_t { None, ReadAndWrite, Readonly };
		enum class PipelineCullMode : uint8_t { None, Back, Front };

		virtual ~Pipeline() = default;
		virtual void init(
			const ShaderHandle& shader,
			const VertexLayout& layout,
			PipelinePrimitives p,
			PipelineScissor s,
			PipelineViewport v,
			PipelineBlendMode bm,
			PipelineDepthMode dm,
			PipelineCullMode cm,
			std::error_code& ec
		) = 0;
		virtual void uninit() = 0;
	};
}