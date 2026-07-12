#include <Rendering/Texture.h>

Texture::~Texture() {
	glDeleteTextures(1, &this->id);
}

void Texture::load_2D_texture(const char* fileName, bool isPixelised) {
	int imgWidth, imgHeight, numOfColorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_bytes = stbi_load(fileName, &imgWidth, &imgHeight, &numOfColorChannels, 0);

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);

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

void Texture::load_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised) {
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, img_buf);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels) {
	int imgWidth, imgHeight;
	stbi_set_flip_vertically_on_load(false);
	*image_bytes = stbi_load(filename, &imgWidth, &imgHeight, &numOfChannels, 0);

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isPixelised ? GL_NEAREST : GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, numOfChannels < 4 ? GL_RGB : GL_RGBA, imgWidth, imgHeight, 0, numOfChannels < 4 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, *image_bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(uint32_t slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}