#pragma once
#include <cstdint>

namespace Engine::Graphics {
	class VBO {
	public:
		enum VBO_USAGE : uint8_t { USAGE_STATIC, USAGE_DYNAMIC };
		uint32_t id = 0;
		VBO();
		VBO(void* data, size_t size);
		~VBO();
		void set_data(void* data, size_t size, VBO_USAGE usage) const;
		void update_data(void* data, size_t size, size_t offset = 0) const;
		void bind_VBO() const;
		void unbind_VBO() const;
		void delete_VBO() const;
	};
}