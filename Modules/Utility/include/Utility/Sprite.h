#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

class Sprite {
public:
	Sprite() {}
	Sprite(float U0, float V0, float width, float height, float base_size) {
		this->U0 = U0;
		this->V0 = V0;
		this->W = width;
		this->H = height;
		float max_size = width > height ? width : height;
		ratio = { width / max_size, height / max_size };
		this->base_size = base_size;
	}
	float U0 = 0.0f, V0 = 0.0f, W = 0.0f, H = 0.0f;
	glm::vec2 ratio{ 1.0f };
	float base_size = 1.0f;
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
	void add_sprite_absolute(float U0, float V0, float width, float height, float base_world_size) {
		sprites.emplace_back(U0, V0, width, height, base_world_size);
	}
	void add_sprite_pixels(uint32_t U0, uint32_t V0, uint32_t width, uint32_t height, float base_world_size) {
		float x0 = float(U0) * MAIN_PIXEL_UV_SIZE;
		float w = float(width) * MAIN_PIXEL_UV_SIZE;
		float y0 = float(V0) * MAIN_PIXEL_UV_SIZE;
		float h = float(height) * MAIN_PIXEL_UV_SIZE;
		sprites.emplace_back(x0, y0, w, h, base_world_size);
	}
	void add_sprite_bordered(float U0_blocks, float V0_blocks, float blocks_width, float blocks_height, float base_world_size) {
		float bordered_block_UV_size = MAIN_BLOCK_UV_SIZE + MAIN_PIXEL_UV_SIZE * 2;
		float x0 = bordered_block_UV_size * U0_blocks + MAIN_PIXEL_UV_SIZE;
		float w = blocks_width * MAIN_BLOCK_UV_SIZE;
		float y0 = 1.0f - bordered_block_UV_size * V0_blocks - bordered_block_UV_size + MAIN_PIXEL_UV_SIZE;
		float h = blocks_height * MAIN_BLOCK_UV_SIZE;
		sprites.emplace_back(x0, y0, w, h, base_world_size);
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