#include "Rendering/OpenGL/gl_TextureArray.h"
#include <glad/gl.h>
#include <stb_image.h>

Engine::Graphics::GL_TextureArray::GL_TextureArray() {
	glGenTextures(1, &id);
}

Engine::Graphics::GL_TextureArray::~GL_TextureArray() {
    if (id) glDeleteTextures(1, &id);
}

void Engine::Graphics::GL_TextureArray::setup_texture_array(
    uint32_t width, uint32_t height, uint32_t depth, FilterMode filter, WrapMode wrap,
    uint8_t channels, ResizeStrategy resize_str, std::error_code& ec
) {
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->filter = filter;
    this->wrap = wrap;
    this->resize_strategy = resize_str;
    this->channels = channels;
    create_tex_array(id);
}

void Engine::Graphics::GL_TextureArray::add_2D_texture(const char* fileName, std::error_code& ec) {
    int imgWidth, imgHeight, numOfColorChannels;
    int desired_channels = (channels == 3) ? 4 : static_cast<int>(channels); //convert RGB to RGBA

    unsigned char* image_bytes = stbi_load(fileName, &imgWidth, &imgHeight, &numOfColorChannels, desired_channels);

    if (!image_bytes) {
        //ec
        return;
    }
    if (imgWidth != width || imgHeight != height) {
        stbi_image_free(image_bytes);
        // ec = ...
        return;
    }

    add_2D_texture(image_bytes, imgWidth, imgHeight, desired_channels, ec);

	stbi_image_free(image_bytes);
}

void Engine::Graphics::GL_TextureArray::add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels, std::error_code& ec) {
    if (current_size == depth) resize_array();

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

void Engine::Graphics::GL_TextureArray::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void Engine::Graphics::GL_TextureArray::unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Engine::Graphics::GL_TextureArray::resize_array() {
    uint32_t new_tex_array_id;
    glGenTextures(1, &new_tex_array_id);

    switch (resize_strategy) {
        case ResizeStrategy::Resize_by_100_percent: depth = (depth == 0) ? 2 : depth *= 2; break;
        case ResizeStrategy::Resize_by_50_percent:  depth = (depth == 0) ? 2 : depth += depth / 2; break;
        case ResizeStrategy::Resize_by_1:           depth += 1; break;
        case ResizeStrategy::Resize_by_2:           depth += 2; break;
        case ResizeStrategy::Resize_by_5:           depth += 5; break;
    }

    create_tex_array(new_tex_array_id);

    if (current_size != 0) {
        glCopyImageSubData(
            this->id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
            new_tex_array_id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
            width, height, current_size
        );
    }

    glDeleteTextures(1, &id);
    id = new_tex_array_id;
}

void Engine::Graphics::GL_TextureArray::create_tex_array(uint32_t tex_ID) const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex_ID);

    // filtering is linear or nearest
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter == FilterMode::Linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter == FilterMode::Linear ? GL_LINEAR : GL_NEAREST);
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
        width, height, depth,
        0,
        format,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}