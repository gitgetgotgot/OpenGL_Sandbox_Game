#include <Utility/TextBufferBuilder.h>

TextBufferBuilder::TextBufferBuilder() {
	buffer_ptr = buffer_data;
}

TextBufferBuilder& TextBufferBuilder::add_text(const char* text) {
	while (*text) {
		*buffer_ptr = *text;
		buffer_ptr++;
		text++;
	}
	return *this;
}

TextBufferBuilder& TextBufferBuilder::add_int(int value) {
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	return *this;
}

TextBufferBuilder& TextBufferBuilder::add_uint64(uint64_t value) {
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	return *this;
}

TextBufferBuilder& TextBufferBuilder::add_float(float value) {
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	return *this;
}

char* TextBufferBuilder::data() {
	*buffer_ptr = '\0';
	return buffer_data;
}

void TextBufferBuilder::reset() {
	buffer_ptr = buffer_data;
}
