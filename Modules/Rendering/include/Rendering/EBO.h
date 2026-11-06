#pragma once
#include <Base/Types.h>

namespace Engine::Graphics {
	class EBO {
	public:
		enum EBO_USAGE : ET::uint8 { USAGE_STATIC, USAGE_DYNAMIC };
		ET::uint32 id = 0;
		EBO();
		EBO(const void* indices, ET::usize size);
		~EBO();
		void set_data(const void* data, ET::usize size, EBO_USAGE usage) const;
		void update_data(const void* data, ET::usize size, ET::usize offset = 0) const;
		void bind_EBO() const;
		void unbind_EBO() const;
		void delete_EBO() const;
	};
}