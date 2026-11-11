#pragma once
#include <cstdint>

namespace Engine::Graphics {
	class Texture {
	public:
		virtual ~Texture() = default;
		virtual void add_2D_texture(const char* fileName, bool isPixelised) = 0;
		virtual void add_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised) = 0;
		virtual void add_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels) = 0;
		virtual void bind(uint32_t slot = 0) const = 0;
		virtual void unbind() const = 0;
	};
}