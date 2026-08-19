#include <Utility/Sprite.h>
#include <iostream>

Sprite::Sprite(float U0, float V0, float width, float height, float base_size, uint32_t texture_array_id) {
	this->U0 = U0;
	this->V0 = V0;
	this->W = width;
	this->H = height;
	float max_size = width > height ? width : height;
	ratio = { width / max_size, height / max_size };
	this->base_size = base_size;
	texture_id = texture_array_id;
}

void SpriteManager::add_sprite(std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height, float base_world_size, uint32_t texture_array_id) {
	float x0 = float(U0) * MAIN_PIXEL_UV_SIZE;
	float w = float(width) * MAIN_PIXEL_UV_SIZE;
	float y0 = float(V0) * MAIN_PIXEL_UV_SIZE;
	float h = float(height) * MAIN_PIXEL_UV_SIZE;
	sprites.emplace_back(x0, y0, w, h, base_world_size, texture_array_id);
	sprite_UID_to_ID.emplace(UID, get_last_index());
}

Sprite& SpriteManager::get_sprite(uint32_t id) {
	return sprites[id];
}

uint32_t SpriteManager::get_sprite_id(std::string uid) {
	auto it = sprite_UID_to_ID.find(uid);
	if (it != sprite_UID_to_ID.end()) {
		return it->second;
	}
	else {
		std::cout << "[SpriteManager]: Sprite " << uid << " not found!" << std::endl;
		return 0;
	}
}

size_t SpriteManager::get_size() {
	return sprites.size();
}

size_t SpriteManager::get_last_index() {
	return sprites.size() - 1;
}