#include "Resources/Sprite.h"
#include "Resources/TextureArrayMgr.h"

Engine::Resource::Sprite::Sprite(
	float U0, float V0, float width, float height,
	float base_size, uint32_t tex_array_id, uint32_t tex_id
) {
	this->U0 = U0;
	this->V0 = V0;
	this->W = width;
	this->H = height;
	float max_size = width > height ? width : height;
	ratio = { width / max_size, height / max_size };
	this->base_size = base_size;
	this->tex_array_id = tex_array_id;
	this->tex_id = tex_id;
}

Engine::Resource::Sprite9Sliced::Sprite9Sliced(
	float U0, float V0, float width, float height,
	float uv_margin_left, float uv_margin_right, float uv_margin_top, float uv_margin_bottom,
	uint32_t tex_array_id, uint32_t tex_id
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

	this->tex_array_id = tex_array_id;
	this->tex_id = tex_id;
}

void Engine::Resource::SpriteManager::setup() {
	// add empty sprites with global_ID = 0
	sprites.emplace_back();
	sprites9sliced.emplace_back();
	UID_to_ID["Core:Empty"] = 0;
}

void Engine::Resource::SpriteManager::ClearData() {
	sprites.resize(MINIMUM_SPRITES_SIZE);
	sprites9sliced.resize(MINIMUM_9SLICED_SPRITES_SIZE);
}

void Engine::Resource::SpriteManager::add_sprite(
	std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
	float base_world_size, uint32_t tex_array_id, uint32_t tex_id
) {
	const float pixel_UV_size = 1.0f / float(Engine::Resource::TextureArrayManager::Instance().get_texture_array_res(tex_array_id));
	const float x0 = float(U0) * pixel_UV_size;
	const float w = float(width) * pixel_UV_size;
	const float y0 = float(V0) * pixel_UV_size;
	const float h = float(height) * pixel_UV_size;
	UID_to_ID[UID] = sprites.size();
	sprites.emplace_back(x0, y0, w, h, base_world_size, tex_array_id, tex_id);
}

Engine::Resource::Sprite& Engine::Resource::SpriteManager::get_sprite(uint32_t id) {
	return sprites[id];
}

std::optional<uint32_t> Engine::Resource::SpriteManager::get_sprite_id(std::string uid) {
	auto it = UID_to_ID.find(uid);
	if (it != UID_to_ID.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}

void Engine::Resource::SpriteManager::add_sprite9sliced(
	std::string UID, uint32_t U0, uint32_t V0, uint32_t width, uint32_t height,
	uint32_t b_left, uint32_t uv_margin_right, uint32_t uv_margin_top, uint32_t b_bottom,
	uint32_t tex_array_id, uint32_t tex_id
) {
	const float pixel_UV_size = 1.0f / float(Engine::Resource::TextureArrayManager::Instance().get_texture_array_res(tex_array_id));
	const float x0 = float(U0) * pixel_UV_size;
	const float w = float(width) * pixel_UV_size;
	const float y0 = float(V0) * pixel_UV_size;
	const float h = float(height) * pixel_UV_size;
	const float border_left = float(b_left) * pixel_UV_size;
	const float border_right = float(uv_margin_right) * pixel_UV_size;
	const float border_top = float(uv_margin_top) * pixel_UV_size;
	const float border_bottom = float(b_bottom) * pixel_UV_size;
	UID_to_ID[UID] = sprites9sliced.size();
	sprites9sliced.emplace_back(x0, y0, w, h, border_left, border_right, border_top, border_bottom, tex_array_id, tex_id);
}

Engine::Resource::Sprite9Sliced& Engine::Resource::SpriteManager::get_sprite9sliced(uint32_t id) {
	return sprites9sliced[id];
}

std::optional<uint32_t> Engine::Resource::SpriteManager::get_sprite9sliced_id(std::string uid) {
	auto it = UID_to_ID.find(uid);
	if (it != UID_to_ID.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}