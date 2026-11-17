#pragma once
#include <cstdint>
#include <system_error>

namespace Engine::Graphics {
	class TextureArray {
	public:
		enum class FilterMode : uint8_t {
			Linear, Nearest
		};
		enum class WrapMode : uint8_t {
			ClampToEdge, ClampToBorder, Repeat, MirroredRepeat
		};
		enum class ResizeStrategy : uint8_t {
			Resize_by_100_percent, Resize_by_50_percent, Resize_by_1, Resize_by_2, Resize_by_5
		};
		virtual ~TextureArray() = default;
		uint32_t get_current_size() const { return current_size; }
		virtual void setup_texture_array(
			uint32_t width, uint32_t height, uint32_t depth, FilterMode filter, WrapMode wrap,
			uint8_t channels, ResizeStrategy resize_str, std::error_code& ec
		) = 0;
		virtual void add_2D_texture(const char* fileName, std::error_code& ec) = 0;
		virtual void add_2D_texture(const unsigned char* img_buf, int& width, int& height, int& numOfChannels, std::error_code& ec) = 0;
	protected:
		uint32_t current_size = 0;
		uint32_t width, height, depth;
		ResizeStrategy resize_strategy = ResizeStrategy::Resize_by_100_percent;
		FilterMode filter = FilterMode::Linear;
		WrapMode wrap = WrapMode::ClampToEdge;
		uint8_t channels = 4;
	};
}