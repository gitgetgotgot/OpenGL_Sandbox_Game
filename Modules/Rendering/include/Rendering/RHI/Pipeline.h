#pragma once
#include "Rendering/RHI/ShaderHandle.h"
#include "Rendering/RHI/VertexLayout.h"
#include "Rendering/RHI/UniformBuffer.h"
#include "Rendering/RHI/StorageBuffer.h"
#include <Rendering/RHI/Texture.h>
#include <Rendering/RHI/TextureArray.h>
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
		// sets are only available in Vulkan (but they are not used in project's shaders, so they are always 0 here)
		virtual void bind_uniform_buffer(UniformBuffer& ub, uint32_t binding = 0, uint32_t set = 0) const = 0;
		virtual void bind_storage_buffer(StorageBuffer& sb, uint32_t binding = 0, uint32_t set = 0) const = 0;
		virtual void bind_sampler_2D(Texture& tex, uint32_t binding = 0, uint32_t set = 0) const = 0;
		virtual void bind_sampler_2D_array(TextureArray& tex_arr, uint32_t binding = 0, uint32_t index = 0, uint32_t set = 0) const = 0;
	};
}