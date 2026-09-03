#include <Utility/Effects.h>

bool CoreResource::Effect::updateEffect(float deltaTime) {
	duration -= deltaTime;
	delta_dmg_time += deltaTime;
	if (duration <= 0.0f) return true; //return true if effect should be removed
	else return false;
}

void CoreResource::EffectsManager::ClearData() {
	UID_storage = {};
	effect_UID_to_ID = {};
	effects = {};
}

void CoreResource::EffectsManager::add_effect_info(EffectType type, std::string uid, EffectStatType stat,
	float effect_value, uint32_t ui_sprite_id, uint32_t particle_id,
	float particleSpawnInterval, float inflict_dmg_cd_time
) {
	std::string_view uid_view = UID_storage.emplace_back(uid);
	effect_UID_to_ID.emplace(uid_view, effects.size());
	effects.emplace_back(type, uid_view, stat, ui_sprite_id, effect_value, particle_id, particleSpawnInterval, inflict_dmg_cd_time);
}

std::optional<uint32_t> CoreResource::EffectsManager::get_effect_id(std::string_view uid) {
	auto it = effect_UID_to_ID.find(uid);
	if (it != effect_UID_to_ID.end())
		return it->second;
	else
		return std::nullopt;
}

CoreResource::EffectData& CoreResource::EffectsManager::get_effect_info(uint32_t id) {
	return effects[id];
}