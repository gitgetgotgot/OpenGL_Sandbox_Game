#include "Rendering/Texture.h"
#include <glad/gl.h>
#include <stb_image.h>

Engine::Graphics::Texture::~Texture() {
	if (id) glDeleteTextures(1, &id);
}

void Engine::Graphics::Texture::add_2D_texture(const char* fileName, bool isPixelised) {
	int imgWidth, imgHeight, numOfColorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_bytes = stbi_load(fileName, &imgWidth, &imgHeight, &numOfColorChannels, 0);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, numOfColorChannels < 4 ? GL_RGB : GL_RGBA, imgWidth, imgHeight, 0, numOfColorChannels < 4 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image_bytes);
	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image_bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::Graphics::Texture::add_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, img_buf);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::Graphics::Texture::add_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels) {
	int imgWidth, imgHeight;
	stbi_set_flip_vertically_on_load(false);
	*image_bytes = stbi_load(filename, &imgWidth, &imgHeight, &numOfChannels, 0);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, numOfChannels < 4 ? GL_RGB : GL_RGBA, imgWidth, imgHeight, 0, numOfChannels < 4 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, *image_bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::Graphics::Texture::bind(uint32_t slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Engine::Graphics::Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}