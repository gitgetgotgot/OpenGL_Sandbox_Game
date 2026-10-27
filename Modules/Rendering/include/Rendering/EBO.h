#pragma once
#include <cstdint>

class EBO {
public:
	enum EBO_USAGE : uint8_t { USAGE_STATIC, USAGE_DYNAMIC };
	uint32_t id = 0;
	EBO();
	EBO(void* indices, size_t size);
	~EBO();
	void set_data(void* indices, size_t size, EBO_USAGE usage) const;
	void update_data(void* indices, size_t size, size_t offset = 0) const;
	void bind_EBO() const;
	void unbind_EBO() const;
	void delete_EBO() const;
};