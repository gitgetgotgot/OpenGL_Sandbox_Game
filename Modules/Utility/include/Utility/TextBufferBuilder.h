#pragma once
#include <charconv>

class TextBufferBuilder {
	char buffer_data[128]{};
	char* buffer_ptr = nullptr;
	int buffer_size = 128;
	uint32_t current_size = 0;
public:
	TextBufferBuilder();
	TextBufferBuilder& add_text(const char* text);
	TextBufferBuilder& add_int(int value);
	TextBufferBuilder& add_uint64(uint64_t value);
	TextBufferBuilder& add_float(float value);
	char* data();
	uint32_t size() const;
	void reset();
};
