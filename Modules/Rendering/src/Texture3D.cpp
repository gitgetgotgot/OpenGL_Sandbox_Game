#include <Rendering/Texture3D.h>

Texture3D::Texture3D() {
	glGenTextures(1, &this->id);
}

Texture3D::~Texture3D() {
	glDeleteTextures(1, &this->id);
}

uint32_t Texture3D::get_current_size() const {
    return current_size;
}

void Texture3D::setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool gl_linear) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        GL_RGBA8,
        width, height, depth,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    this->array_width = width;
    this->array_height = height;
    this->array_depth = depth;

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, gl_linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, gl_linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

bool Texture3D::add_2D_texture(const char* fileName)
{
    if (current_size == array_depth) return false;

	int imgWidth, imgHeight, numOfColorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_bytes = stbi_load(fileName, &imgWidth, &imgHeight, &numOfColorChannels, 0);

    if (imgWidth != array_width || imgHeight != array_height) {
        stbi_set_flip_vertically_on_load(false);
        stbi_image_free(image_bytes);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->id);
	glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        0, 0, current_size,
        imgWidth, imgHeight, 1,
        numOfColorChannels < 4 ? GL_RGB : GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_bytes);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    stbi_set_flip_vertically_on_load(false);
	stbi_image_free(image_bytes);
    current_size++;

    return true;
}

void Texture3D::add_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised) {

}

void Texture3D::add_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels) {

}

void Texture3D::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void Texture3D::unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
