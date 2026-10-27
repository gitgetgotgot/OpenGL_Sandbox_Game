#pragma once
#include <cstdint>

namespace Engine::Graphics {
	class SSBO {
	public:
		enum SSBO_USAGE : uint8_t { USAGE_STATIC, USAGE_DYNAMIC };
		uint32_t id = 0;
		SSBO();
		~SSBO();
		void set_data(void* data, size_t size, SSBO_USAGE usage) const;
		void update_data(void* data, size_t size, size_t offset = 0) const;
		void set_persistent_storage_data(void* data, size_t size) const;
		void bind_SSBO(uint32_t slot = 0) const;
		void unbind_SSBO() const;
		void delete_SSBO() const;
	};
}