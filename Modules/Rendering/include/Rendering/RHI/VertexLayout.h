#pragma once
#include <cstdint>
#include <cstddef>

namespace Engine::Graphics {
	enum class VertexAttributeType : uint8_t {
		FLOAT,
		FLOAT_VEC2,
		FLOAT_VEC3,
		FLOAT_VEC4,
		UINT,
		UINT_VEC2,
		UINT_VEC3,
		UINT_VEC4,
		INT,
		INT_VEC2,
		INT_VEC3,
		INT_VEC4
	};
	struct VertexAttributeElement {
		uint32_t location;
		VertexAttributeType type;
		size_t stride;
		size_t offset;
	};
	class VertexLayout {
	public:
		virtual ~VertexLayout() = default;
		virtual void add_attribute(uint32_t location, VertexAttributeType type, size_t stride, size_t offset) = 0;
		virtual void uninit() = 0;
	};
}