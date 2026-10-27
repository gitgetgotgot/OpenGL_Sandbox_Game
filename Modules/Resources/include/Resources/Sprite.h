#pragma once
#include <Utility/DataParser.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <optional>

namespace Engine::Resource {
	class Sprite {
	public:
		Sprite() {}
		Sprite(
			float U0, float V0, float width, float height,
			float base_size, uint32_t tex_array_id, uint32_t tex_id
		);
		uint32_t tex_array_id = 0;
		uint32_t tex_id = 0;
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
			uint32_t tex_array_id, uint32_t tex_id
		);
		uint32_t tex_array_id = 0;
		uint32_t tex_id = 0;
		float U0 = 0.0f, V0 = 0.0f, W = 0.0f, H = 0.0f;
		float uv_margin_left = 0.0f;
		float uv_margin_right = 0.0f;
		float uv_margin_top = 0.0f;
		float uv_margin_bottom = 0.0f;
		float ratio_left = 0.0f;
		float ratio_right = 0.0f;
		float ratio_top = 0.0f;
		float ratio_bottom = 0.0f;
	};

	class SpriteManager {
	public:
		static SpriteManager& Instance() {
			static SpriteManager spriteMgr;
			return spriteMgr;
		}
		void setup();
		void ClearData();
		void add_sprite(
			std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
			float base_world_size, uint32_t tex_array_id, uint32_t tex_id
		);
		Sprite& get_sprite(uint32_t id);
		std::optional<uint32_t> get_sprite_id(std::string uid);

		void add_sprite9sliced(
			std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
			uint32_t b_left, uint32_t uv_margin_right, uint32_t uv_margin_top, uint32_t uv_margin_bottom,
			uint32_t tex_array_id, uint32_t tex_id
		);
		Sprite9Sliced& get_sprite9sliced(uint32_t id);
		std::optional<uint32_t> get_sprite9sliced_id(std::string uid);
	private:
		SpriteManager() {}
		~SpriteManager() {}
		std::unordered_map<std::string, uint32_t> UID_to_ID;
		std::vector<Sprite> sprites;
		std::vector<Sprite9Sliced> sprites9sliced;

		uint8_t MINIMUM_9SLICED_SPRITES_SIZE = 1;
		uint8_t MINIMUM_SPRITES_SIZE = 1;
	};
}