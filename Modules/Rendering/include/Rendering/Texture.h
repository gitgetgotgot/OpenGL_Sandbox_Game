#pragma once
#include <cstdint>

namespace Engine::Graphics {
	class Texture {
		uint32_t id = 0;
	public:
		~Texture();
		void add_2D_texture(const char* fileName, bool isPixelised);
		void add_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised);
		void add_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels);
		void bind(uint32_t slot = 0) const;
		void unbind() const;
	};
}