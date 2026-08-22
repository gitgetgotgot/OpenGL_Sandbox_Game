#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

class Sprite {
public:
	Sprite() {}
	Sprite(float U0, float V0, float width, float height, float base_size, uint32_t texture_array_id);
	uint32_t texture_id = 0; //index in texture array [0, N]
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
	void add_sprite(std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height, float base_world_size, uint32_t texture_array_id);
	Sprite& get_sprite(uint32_t id);
	uint32_t get_sprite_id(std::string name);
	size_t get_size();
	size_t get_last_index();
	inline static float MAIN_BLOCK_UV_SIZE;
	inline static float MAIN_PIXEL_UV_SIZE;
private:
	SpriteManager(){}
	~SpriteManager(){}
	std::unordered_map<std::string/*UID*/, uint32_t/*sprite_id*/> sprite_UID_to_ID;
	std::vector<Sprite> sprites;
};