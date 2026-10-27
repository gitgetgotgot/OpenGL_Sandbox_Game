#pragma once
#include <cstdint>

namespace Engine::Graphics {
	class VAO {
	public:
		enum VAO_ATTRIBUTE_TYPE : uint8_t { TYPE_FLOAT, TYPE_UNSIGNED_INT };
		uint32_t id = 0;
		VAO();
		~VAO();
		void link_Attribute(uint32_t layout, uint32_t comps, VAO_ATTRIBUTE_TYPE type, size_t stride, size_t offset) const;
		void bind_VAO() const;
		void unbind_VAO() const;
		void delete_VAO() const;
	};
}