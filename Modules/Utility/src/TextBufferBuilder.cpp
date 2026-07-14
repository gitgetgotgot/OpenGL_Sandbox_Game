#include <Utility/TextBufferBuilder.h>

TextBufferBuilder::TextBufferBuilder() {
	buffer_ptr = buffer_data;
}

TextBufferBuilder& TextBufferBuilder::add_text(const char* text) {
	while (*text) {
		*buffer_ptr = *text;
		buffer_ptr++;
		current_size++;
		text++;
	}
	return *this;
}

TextBufferBuilder& TextBufferBuilder::add_int(int value) {
	char* old_ptr = buffer_ptr;
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	current_size += buffer_ptr - old_ptr;
	return *this;
}

TextBufferBuilder& TextBufferBuilder::add_uint64(uint64_t value) {
	char* old_ptr = buffer_ptr;
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	current_size += buffer_ptr - old_ptr;
	return *this;
}

TextBufferBuilder& TextBufferBuilder::add_float(float value) {
	char* old_ptr = buffer_ptr;
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	current_size += buffer_ptr - old_ptr;
	return *this;
}

char* TextBufferBuilder::data() {
	*buffer_ptr = '\0';
	return buffer_data;
}

uint32_t TextBufferBuilder::size() const {
	return current_size;
}

void TextBufferBuilder::reset() {
	buffer_ptr = buffer_data;
	current_size = 0;
}
