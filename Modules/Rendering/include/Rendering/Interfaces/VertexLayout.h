#pragma once
#include <cstdint>
#include <cstddef>

namespace Engine::Graphics {
	class VertexLayout {
	public:
		enum ATTRIBUTE_TYPE : uint8_t { TYPE_FLOAT, TYPE_UNSIGNED_INT };
		virtual void link_Attribute(uint32_t layout, uint32_t comps, ATTRIBUTE_TYPE type, size_t stride, size_t offset) const = 0;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void uninit() const = 0;
	};
}