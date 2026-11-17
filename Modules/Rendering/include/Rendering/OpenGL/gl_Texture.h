#pragma once
#include "Rendering/RHI/Texture.h"

namespace Engine::Graphics {
	class GL_Texture : public Texture {
	public:
		~GL_Texture();
		void add_2D_texture(const char* fileName, bool isPixelised) override;
		void add_R8_texture_from_buffer(const unsigned char* img_buf, int width, int height, bool isPixelised) override;
		void add_text_bitmap(const char* filename, bool isPixelised, unsigned char** image_bytes, int& numOfChannels) override;
		void bind(uint32_t slot = 0) const;
		void unbind() const;
	private:
		uint32_t id = 0;
	};
}