#include "Resources/Sprite.h"
#include "Resources/TextureArrayMgr.h"
#include <Utility/Logger.h>

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

void Engine::Resource::SpriteManager::RememberBaseData() {
	MINIMUM_9SLICED_SPRITES_SIZE = sprites9sliced.size();
	MINIMUM_SPRITES_SIZE = sprites.size();
}

void Engine::Resource::SpriteManager::ClearData() {
	sprites.resize(MINIMUM_SPRITES_SIZE);
	sprites9sliced.resize(MINIMUM_9SLICED_SPRITES_SIZE);
}

bool Engine::Resource::SpriteManager::load_sprites_from_data(
	std::filesystem::path& data_file_path,
	Engine::Util::DataParser& parser,
	std::mutex& resources_mutex
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		std::string UID, texture_UID;
		uint32_t padding, u0, v0, w, h;
		nlohmann::json sprite;

		bool ok = true;
		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(UID, j, "UID");
		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(texture_UID, j, "texture");

		auto tex_id_opt = Engine::Resource::TextureArrayManager::Instance().get_texture_id(texture_UID);
		if (!tex_id_opt.has_value()) {
			Engine::Util::Logger::Log("Couldn't find referenced resource <" + texture_UID + "> for Sprite <" + UID + "> in <" + data_file_path.string() + ">!");
			return false;
		}
		uint32_t tex_array_id = tex_id_opt.value().array_id;
		uint32_t tex_id = tex_id_opt.value().index;

		ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(padding, j, "padding");
		ok &= DP::get_json_value<nlohmann::json, DP::JsonValue::V_OBJECT>(sprite, j, "sprite");
		ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(u0, sprite, "U0");
		ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(v0, sprite, "V0");
		ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(w, sprite, "w");
		ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(h, sprite, "h");

		if (sprite.contains("border")) {
			nlohmann::json border;
			uint32_t left, right, top, bottom;
			ok &= DP::get_json_value<nlohmann::json, DP::JsonValue::V_OBJECT>(border, sprite, "border");
			ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(left, border, "left");
			ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(right, border, "right");
			ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(top, border, "top");
			ok &= DP::get_json_value<uint32_t, DP::JsonValue::V_UINT>(bottom, border, "bottom");

			if (ok) {
				std::lock_guard lock(resources_mutex);
				add_sprite9sliced(UID, u0, v0, w, h, left, right, top, bottom, tex_array_id, tex_id);
			}
			else {
				Engine::Util::Logger::Log("Invalid syntax for Sprite in <" + data_file_path.string() + ">!");
				return false;
			}
		}
		else {
			float base_size;
			ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(base_size, sprite, "base_size");

			if (ok) {
				std::lock_guard lock(resources_mutex);
				add_sprite(UID, u0, v0, w, h, base_size, tex_array_id, tex_id);
			}
			else {
				Engine::Util::Logger::Log("Invalid syntax found for Sprite in <" + data_file_path.string() + ">!");
				return false;
			}
		}
	}
	return true;
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