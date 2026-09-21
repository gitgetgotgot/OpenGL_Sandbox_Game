#include "Resources/Sprite.h"

CoreResource::Sprite::Sprite(
	float U0, float V0, float width, float height,
	float base_size, uint32_t texture_array_id
) {
	this->U0 = U0;
	this->V0 = V0;
	this->W = width;
	this->H = height;
	float max_size = width > height ? width : height;
	ratio = { width / max_size, height / max_size };
	this->base_size = base_size;
	texture_id = texture_array_id;
}

CoreResource::Sprite9Sliced::Sprite9Sliced(
	float U0, float V0, float width, float height,
	float uv_margin_left, float uv_margin_right, float uv_margin_top, float uv_margin_bottom,
	uint32_t texture_array_id
) {
	this->U0 = U0;
	this->V0 = V0;
	this->W = width;
	this->H = height;

	this->uv_margin_left = uv_margin_left;
	this->uv_margin_right = uv_margin_right;
	this->uv_margin_top = uv_margin_top;
	this->uv_margin_bottom = uv_margin_bottom;

	float max_uv_margin = std::max(uv_margin_left, std::max(uv_margin_right, std::max(uv_margin_top, uv_margin_bottom)));
	ratio_left = max_uv_margin / uv_margin_left;
	ratio_right = max_uv_margin / uv_margin_right;
	ratio_top = max_uv_margin / uv_margin_top;
	ratio_bottom = max_uv_margin / uv_margin_bottom;

	texture_id = texture_array_id;
}

void CoreResource::SpriteManager::ClearData() {
	sprites = {};
	sprite_UID_to_ID = {};
	sprites9sliced = {};
	sprite9sliced_UID_to_ID = {};
}

void CoreResource::SpriteManager::add_sprite(
	std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
	float base_world_size, uint32_t texture_array_id
) {
	float x0 = float(U0) * MAIN_PIXEL_UV_SIZE;
	float w = float(width) * MAIN_PIXEL_UV_SIZE;
	float y0 = float(V0) * MAIN_PIXEL_UV_SIZE;
	float h = float(height) * MAIN_PIXEL_UV_SIZE;
	sprite_UID_to_ID.emplace(UID, sprites.size());
	sprites.emplace_back(x0, y0, w, h, base_world_size, texture_array_id);
}

CoreResource::Sprite& CoreResource::SpriteManager::get_sprite(uint32_t id) {
	return sprites[id];
}

std::optional<uint32_t> CoreResource::SpriteManager::get_sprite_id(std::string uid) {
	auto it = sprite_UID_to_ID.find(uid);
	if (it != sprite_UID_to_ID.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}

void CoreResource::SpriteManager::add_sprite9sliced(
	std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
	uint32_t b_left, uint32_t uv_margin_right, uint32_t uv_margin_top, uint32_t b_bottom,
	uint32_t texture_array_id
) {
	float x0 = float(U0) * MAIN_PIXEL_UV_SIZE;
	float w = float(width) * MAIN_PIXEL_UV_SIZE;
	float y0 = float(V0) * MAIN_PIXEL_UV_SIZE;
	float h = float(height) * MAIN_PIXEL_UV_SIZE;
	float border_left = float(b_left) * MAIN_PIXEL_UV_SIZE;
	float border_right = float(uv_margin_right) * MAIN_PIXEL_UV_SIZE;
	float border_top = float(uv_margin_top) * MAIN_PIXEL_UV_SIZE;
	float border_bottom = float(b_bottom) * MAIN_PIXEL_UV_SIZE;
	sprite9sliced_UID_to_ID.emplace(UID, sprites9sliced.size());
	sprites9sliced.emplace_back(x0, y0, w, h, border_left, border_right, border_top, border_bottom, texture_array_id);
}

CoreResource::Sprite9Sliced& CoreResource::SpriteManager::get_sprite9sliced(uint32_t id) {
	return sprites9sliced[id];
}

std::optional<uint32_t> CoreResource::SpriteManager::get_sprite9sliced_id(std::string uid) {
	auto it = sprite9sliced_UID_to_ID.find(uid);
	if (it != sprite9sliced_UID_to_ID.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}