#include "Resources/Effect.h"
#include <Resources/Sprite.h>
#include <Utility/Logger.h>

bool Engine::Resource::Effect::updateEffect(float deltaTime) {
	duration -= deltaTime;
	delta_dmg_time += deltaTime;
	if (duration <= 0.0f) return true; //return true if effect should be removed
	else return false;
}

void Engine::Resource::EffectsManager::setup() {
	// add empty effect with global_ID = 0
	effects.emplace_back();
	effect_UID_to_ID["Core:Empty"] = 0;
}

void Engine::Resource::EffectsManager::ClearData() {
	UID_storage = {};
	effect_UID_to_ID = {};
	effects = {};
}

bool Engine::Resource::EffectsManager::load_effects_from_data(
	std::filesystem::path& data_file_path,
	Engine::Util::DataParser& parser
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		std::string UID, sprite_UID;
		uint8_t effect_type_id, stat_type_id;
		float value;
		float p_spawn_cd = 0.0f, dmg_cd = 0.0f;
		uint32_t particle_id = 0;
		bool ok = true;

		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(UID, j, "UID");
		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(sprite_UID, j, "sprite_UID");
		ok &= DP::get_json_value<uint8_t, DP::JsonValue::V_UINT>(effect_type_id, j, "effect_type");
		ok &= DP::get_json_value<uint8_t, DP::JsonValue::V_UINT>(stat_type_id, j, "stat_type");
		ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(value, j, "value");
		if (j.contains("dmg_cd")) {
			ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(dmg_cd, j, "dmg_cd");
		}
		if (j.contains("particle")) {
			auto& p = j["particle"];

		}
		auto sprite_global_id = Engine::Resource::SpriteManager::Instance().get_sprite_id(sprite_UID);
		if (!sprite_global_id) {
			Engine::Util::Logger::Log("Couldn't find referenced resource <" + sprite_UID + "> for Effect <" + UID + "> in <" + data_file_path.string() + ">!");
			return false;
		}

		if (ok) {
			Engine::Resource::EffectsManager::Instance().add_effect_info(
				(Engine::Resource::Effect::EffectType)effect_type_id, UID,
				(Engine::Resource::Effect::EffectStatType)stat_type_id, value,
				sprite_global_id.value(), particle_id, p_spawn_cd, dmg_cd
			);
		}
		else {
			Engine::Util::Logger::Log("Invalid syntax for Effect in <" + data_file_path.string() + ">!");
			return false;
		}
	}
	return true;
}

void Engine::Resource::EffectsManager::add_effect_info(Effect::EffectType type, std::string uid, Effect::EffectStatType stat,
	float effect_value, uint32_t ui_sprite_id, uint32_t particle_id,
	float particleSpawnInterval, float inflict_dmg_cd_time
) {
	std::string_view uid_view = UID_storage.emplace_back(uid);
	effect_UID_to_ID.emplace(uid_view, effects.size());
	effects.emplace_back(type, uid_view, stat, ui_sprite_id, effect_value, particle_id, particleSpawnInterval, inflict_dmg_cd_time);
}

std::optional<uint32_t> Engine::Resource::EffectsManager::get_effect_id(std::string_view uid) {
	auto it = effect_UID_to_ID.find(uid);
	if (it != effect_UID_to_ID.end())
		return it->second;
	else
		return std::nullopt;
}

Engine::Resource::EffectData& Engine::Resource::EffectsManager::get_effect_info(uint32_t id) {
	return effects[id];
}