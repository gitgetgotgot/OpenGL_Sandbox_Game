#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <optional>

namespace CoreResource {
	class Sprite {
	public:
		Sprite() {}
		Sprite(
			float U0, float V0, float width, float height,
			float base_size, uint32_t texture_array_id
		);
		uint32_t texture_id = 0; //index in texture array [0, N]
		float U0 = 0.0f, V0 = 0.0f, W = 0.0f, H = 0.0f;
		glm::vec2 ratio{ 1.0f };
		float base_size = 1.0f;
	};

	class Sprite9Sliced {
	public:
		Sprite9Sliced() {}
		Sprite9Sliced(
			float U0, float V0, float width, float height,
			float uv_margin_left, float uv_margin_right, float uv_margin_top, float uv_margin_bottom,
			uint32_t texture_array_id
		);
		uint32_t texture_id = 0; //index in texture array [0, N]
		float U0 = 0.0f, V0 = 0.0f, W = 0.0f, H = 0.0f;
		float
			uv_margin_left = 0.0f,
			uv_margin_right = 0.0f,
			uv_margin_top = 0.0f,
			uv_margin_bottom = 0.0f;
		float
			ratio_left = 0.0f,
			ratio_right = 0.0f,
			ratio_top = 0.0f,
			ratio_bottom = 0.0f;
	};

	class SpriteManager {
	public:
		static SpriteManager& get_instance() {
			static SpriteManager spriteMgr;
			return spriteMgr;
		}
		void ClearData();
		void add_sprite(
			std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
			float base_world_size, uint32_t texture_array_id
		);
		Sprite& get_sprite(uint32_t id);
		std::optional<uint32_t> get_sprite_id(std::string uid);

		void add_sprite9sliced(
			std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
			uint32_t b_left, uint32_t uv_margin_right, uint32_t uv_margin_top, uint32_t uv_margin_bottom,
			uint32_t texture_array_id
		);
		Sprite9Sliced& get_sprite9sliced(uint32_t id);
		std::optional<uint32_t> get_sprite9sliced_id(std::string uid);

		inline static float MAIN_PIXEL_UV_SIZE;
	private:
		SpriteManager() {}
		~SpriteManager() {}
		std::unordered_map<std::string, uint32_t> sprite_UID_to_ID;
		std::vector<Sprite> sprites;
		std::unordered_map<std::string, uint32_t> sprite9sliced_UID_to_ID;
		std::vector<Sprite9Sliced> sprites9sliced;
	};
}