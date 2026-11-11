#pragma once
#include "Rendering/Interfaces/TextureArray.h"

namespace Engine::Graphics {
	class gl_TextureArray : public TextureArray {
	public:
		gl_TextureArray();
		~gl_TextureArray();
		void bind(uint32_t slot = 0) const override;
		void unbind() const override;
		uint32_t get_current_size() const override;
		void setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool linear_filter, uint8_t channels, ResizeStrategy resize_str) override;
		bool add_2D_texture(const char* fileName) override;
		void add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels) override;
	private:
		void resize_array();
		void create_tex_array(uint32_t tex_ID) const;
		uint32_t id = 0;
	};
}