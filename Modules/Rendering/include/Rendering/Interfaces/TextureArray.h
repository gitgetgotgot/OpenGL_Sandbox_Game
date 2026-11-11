#pragma once
#include <cstdint>

namespace Engine::Graphics {
	class TextureArray {
	public:
		enum ResizeStrategy : uint8_t {
			Resize_by_100_percent, Resize_by_50_percent, Resize_by_1, Resize_by_2, Resize_by_5
		};
		virtual ~TextureArray() = default;
		virtual void bind(uint32_t slot = 0) const = 0;
		virtual void unbind() const = 0;
		virtual uint32_t get_current_size() const = 0;
		virtual void setup_texture_array(uint32_t width, uint32_t height, uint32_t depth, bool linear_filter, uint8_t channels, ResizeStrategy resize_str) = 0;
		virtual bool add_2D_texture(const char* fileName) = 0;
		virtual void add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels) = 0;
	protected:
		uint32_t current_size = 0;
		uint32_t array_width, array_height, array_depth;
		ResizeStrategy resize_strategy = ResizeStrategy::Resize_by_100_percent;
		bool linear_filter = false;
		uint8_t channels = 4;
	};
}