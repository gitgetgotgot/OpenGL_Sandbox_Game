#pragma once
#include <cstdint>

class UBO {
public:
	enum UBO_USAGE : uint8_t { USAGE_STATIC, USAGE_DYNAMIC };
	uint32_t id = 0;
	UBO();
	~UBO();
	void set_data(void* data, size_t size, UBO_USAGE usage) const;
	void update_data(void* data, size_t size, size_t offset = 0) const;
	void bind_UBO(uint32_t slot = 0) const;
	void unbind_UBO() const;
	void delete_UBO() const;
};