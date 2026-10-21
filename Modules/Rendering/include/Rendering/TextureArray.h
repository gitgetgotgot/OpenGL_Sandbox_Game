#pragma once
#include <cstdint>

class TextureArray {
public:
	enum ResizeStrategy : uint8_t {
		Resize_by_100_percent, Resize_by_50_percent, Resize_by_1, Resize_by_2, Resize_by_5
	};
	TextureArray();
	~TextureArray();

	void bind(uint32_t slot = 0) const;
	void unbind() const;
	uint32_t get_current_size() const;
	void setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool linear_filter, uint8_t channels, ResizeStrategy resize_str);

	bool add_2D_texture(const char* fileName);
	// add texture with preloaded buffer
	void add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels);
private:
	void resize_array();
	void create_tex_array(uint32_t tex_ID) const;
	uint32_t id;
	uint32_t current_size = 0;
	uint32_t array_width, array_height, array_depth;
	ResizeStrategy resize_strategy = ResizeStrategy::Resize_by_100_percent;
	bool linear_filter = false;
	uint8_t channels = 4;
};