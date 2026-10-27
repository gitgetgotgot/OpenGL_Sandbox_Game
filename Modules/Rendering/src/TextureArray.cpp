#include "Rendering/TextureArray.h"
#include <glad/gl.h>
#include <stb_image.h>

TextureArray::TextureArray() {
	glGenTextures(1, &id);
}

TextureArray::~TextureArray() {
    if (id) glDeleteTextures(1, &id);
}

void TextureArray::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void TextureArray::unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

uint32_t TextureArray::get_current_size() const {
    return current_size;
}

void TextureArray::setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool linear_filter, uint8_t channels, ResizeStrategy resize_str) {
    this->array_width = width;
    this->array_height = height;
    this->array_depth = depth;
    this->linear_filter = linear_filter;
    this->resize_strategy = resize_str;
    this->channels = channels;
    create_tex_array(id);
}

bool TextureArray::add_2D_texture(const char* fileName) {
    if (current_size == array_depth) resize_array();

	int imgWidth, imgHeight, numOfColorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_bytes = stbi_load(fileName, &imgWidth, &imgHeight, &numOfColorChannels, 0);

    if (imgWidth != array_width || imgHeight != array_height) {
        stbi_set_flip_vertically_on_load(false);
        stbi_image_free(image_bytes);
        return false;
    }

    GLenum format;
    if (numOfColorChannels == 1) format = GL_RED;
    else if (numOfColorChannels == 2) format = GL_RG;
    else if (numOfColorChannels == 3) format = GL_RGB;
    else format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->id);
	glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        0, 0, current_size,
        imgWidth, imgHeight, 1,
        format,
        GL_UNSIGNED_BYTE,
        image_bytes
    );
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    stbi_set_flip_vertically_on_load(false);
	stbi_image_free(image_bytes);
    ++current_size;

    return true;
}

void TextureArray::add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels) {
    if (width != array_width || height != array_height) return;

    if (current_size == array_depth) resize_array();

    GLenum format;
    if (numOfChannels == 1) format = GL_RED;
    else if (numOfChannels == 2) format = GL_RG;
    else if (numOfChannels == 3) format = GL_RGB;
    else format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        0, 0, current_size,
        width, height, 1,
        format,
        GL_UNSIGNED_BYTE,
        img_buf
    );
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    ++current_size;
}

void TextureArray::resize_array() {
    uint32_t new_tex_array_id;
    glGenTextures(1, &new_tex_array_id);

    switch (resize_strategy) {
    case ResizeStrategy::Resize_by_100_percent: {
        if (array_depth == 0)
            array_depth = 2;
        else
            array_depth *= 2;
        break;
    }
    case ResizeStrategy::Resize_by_50_percent: {
        if (array_depth == 0)
            array_depth = 2;
        else
            array_depth += array_depth / 2;
        break;
    }
    case ResizeStrategy::Resize_by_1: {
        array_depth += 1;
        break;
    }
    case ResizeStrategy::Resize_by_2: {
        array_depth += 2;
        break;
    }
    case ResizeStrategy::Resize_by_5: {
        array_depth += 5;
        break;
    }
    }

    create_tex_array(new_tex_array_id);

    if (current_size != 0) {
        glCopyImageSubData(
            this->id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
            new_tex_array_id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
            array_width, array_height, current_size
        );
    }

    glDeleteTextures(1, &id);
    id = new_tex_array_id;
}

void TextureArray::create_tex_array(uint32_t tex_ID) const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex_ID);

    // filtering is linear or nearest
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, linear_filter ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, linear_filter ? GL_LINEAR : GL_NEAREST);
    // clamp texture to edge with no repeat
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set max mipmapping level to 0 (no mipmaps will be generated)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

    GLint internal_format;
    GLenum format;
    if (channels == 1) {
        internal_format = GL_R8;
        format = GL_RED;
    }
    else if (channels == 2) {
        internal_format = GL_RG8;
        format = GL_RG;
    }
    else if (channels == 3) {
        internal_format = GL_RGB8;
        format = GL_RGB;
    }
    else {
        internal_format = GL_RGBA8;
        format = GL_RGBA;
    }

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        internal_format,
        array_width, array_height, array_depth,
        0,
        format,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}