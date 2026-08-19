#include <glad/glad.h>
#include <stb/stb_image.h>
#include <cstdint>

class Texture3D {
	GLuint id;
	uint32_t current_size = 0;
	uint32_t array_width, array_height, array_depth;
public:
	Texture3D();
	~Texture3D();
	uint32_t get_current_size() const;
	void setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool gl_linear);
	bool add_2D_texture(const char* fileName);
	void add_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised);
	void add_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels);
	void bind(uint32_t slot = 0) const;
	void unbind() const;
};