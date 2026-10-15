#include <Rendering/TextureArray.h>
#include <glad/gl.h>
#include <stb_image.h>

TextureArray::TextureArray() {
	glGenTextures(1, &this->id);
}

TextureArray::~TextureArray() {
	glDeleteTextures(1, &this->id);
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

void TextureArray::setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool linear_filter, ResizeStrategy resize_str) {
    this->array_width = width;
    this->array_height = height;
    this->array_depth = depth;
    this->linear_filter = linear_filter;
    this->resize_strategy = resize_str;
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

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->id);
	glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        0, 0, current_size,
        imgWidth, imgHeight, 1,
        numOfColorChannels < 4 ? GL_RGB : GL_RGBA,
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

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->id);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        0, 0, current_size,
        width, height, 1,
        numOfChannels < 4 ? GL_RGB : GL_RGBA,
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

    glDeleteTextures(1, &this->id);
    this->id = new_tex_array_id;
}

void TextureArray::create_tex_array(uint32_t tex_ID) const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex_ID);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0,
        GL_RGBA8,
        array_width, array_height, array_depth,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, linear_filter ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, linear_filter ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}