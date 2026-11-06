#include <Utility/TextBufferBuilder.h>
#include <charconv>

Engine::Util::TextBufferBuilder::TextBufferBuilder() {
	buffer_ptr = buffer_data;
}

Engine::Util::TextBufferBuilder& Engine::Util::TextBufferBuilder::add_text(const char* text) {
	while (*text) {
		*buffer_ptr = *text;
		buffer_ptr++;
		current_size++;
		text++;
	}
	return *this;
}

Engine::Util::TextBufferBuilder& Engine::Util::TextBufferBuilder::add_int(int value) {
	char* old_ptr = buffer_ptr;
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	current_size += buffer_ptr - old_ptr;
	return *this;
}

Engine::Util::TextBufferBuilder& Engine::Util::TextBufferBuilder::add_uint64(uint64_t value) {
	char* old_ptr = buffer_ptr;
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	current_size += buffer_ptr - old_ptr;
	return *this;
}

Engine::Util::TextBufferBuilder& Engine::Util::TextBufferBuilder::add_float(float value) {
	char* old_ptr = buffer_ptr;
	std::to_chars_result result = std::to_chars(buffer_ptr, buffer_data + buffer_size, value);
	buffer_ptr = result.ptr;
	current_size += buffer_ptr - old_ptr;
	return *this;
}

char* Engine::Util::TextBufferBuilder::data() {
	*buffer_ptr = '\0';
	return buffer_data;
}

uint32_t Engine::Util::TextBufferBuilder::size() const {
	return current_size;
}

void Engine::Util::TextBufferBuilder::reset() {
	buffer_ptr = buffer_data;
	current_size = 0;
}