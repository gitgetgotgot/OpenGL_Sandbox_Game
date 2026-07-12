#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <cstdint>

class Texture {
	GLuint id;
public:
	~Texture();
	void load_2D_texture(const char* fileName, bool isPixelised);
	void load_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised);
	void load_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels);
	void bind(uint32_t slot = 0) const;
	void unbind() const;
};