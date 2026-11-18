#pragma once
#include "Rendering/RHI/TextureArray.h"

namespace Engine::Graphics {
	class GL_TextureArray : public TextureArray {
	public:
		GL_TextureArray();
		~GL_TextureArray();
		void setup_texture_array(
			uint32_t width, uint32_t height, uint32_t depth, FilterMode filter, WrapMode wrap,
			uint8_t channels, ResizeStrategy resize_str, std::error_code& ec
		) override;
		void add_2D_texture(const char* fileName, std::error_code& ec) override;
		void add_2D_texture(const unsigned char* img_buf, int width, int height, int numOfChannels, std::error_code& ec) override;

		void bind(uint32_t slot = 0);
	private:
		void resize_array();
		void create_tex_array(uint32_t tex_ID) const;
		uint32_t id = 0;
		uint32_t bound_slot = 0;
	};
}