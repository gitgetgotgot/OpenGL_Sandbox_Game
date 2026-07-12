#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

class Sprite {
public:
	Sprite() {
		UV[0] = { 0.0f, 0.0f };
		UV[1] = { 0.0f, 1.0f };
		UV[2] = { 1.0f, 1.0f };
		UV[3] = { 1.0f, 0.0f };
	}
	Sprite(float U0, float V0, float width, float height) {
		UV[0] = { U0,         V0          };
		UV[1] = { U0,         V0 + height };
		UV[2] = { U0 + width, V0 + height };
		UV[3] = { U0 + width, V0          };
	}
	glm::vec2 UV[4]{};
};

class SpriteManager {
public:
	static SpriteManager* get_instance() {
		static SpriteManager spriteMgr;
		return &spriteMgr;
	}
	void add_sprite() {
		sprites.emplace_back();
	}
	void add_sprite(float U0, float V0, float width, float height) {
		sprites.emplace_back(U0, V0, width, height);
	}
	void add_sprite_bordered(float U0_blocks, float V0_blocks, float blocks_width, float blocks_height) {
		float bordered_block_UV_size = MAIN_BLOCK_UV_SIZE + MAIN_PIXEL_UV_SIZE * 2;
		float x0 = bordered_block_UV_size * U0_blocks + MAIN_PIXEL_UV_SIZE;
		float w = blocks_width * MAIN_BLOCK_UV_SIZE;
		float y0 = 1.0f - bordered_block_UV_size * V0_blocks - bordered_block_UV_size + MAIN_PIXEL_UV_SIZE;
		float h = blocks_height * MAIN_BLOCK_UV_SIZE;
		sprites.emplace_back(x0, y0, w, h);
	}
	Sprite& get_sprite(uint32_t id) {
		return sprites[id];
	}
	size_t get_size() {
		return sprites.size();
	}
	size_t get_last_index() {
		return sprites.size() - 1;
	}
	inline static float MAIN_BLOCK_UV_SIZE;
	inline static float MAIN_PIXEL_UV_SIZE;
private:
	SpriteManager(){}
	~SpriteManager(){}
	std::vector<Sprite> sprites;
};