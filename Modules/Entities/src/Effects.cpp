#include <Entities/Effects.h>

bool Effects::Effect::updateEffect(float deltaTime) {
	duration -= deltaTime;
	delta_dmg_time += deltaTime;
	if (duration <= 0.0f) return true; //return true if effect should be removed
	else return false;
}

void Effects::EffectsManager::init(uint32_t effects_size) {
	effects_data.reserve(effects_size);
}

void Effects::EffectsManager::add_effect_info(EffectType type, std::string uid, StatType stat,
	float effect_value, uint32_t ui_sprite_id, uint32_t particle_id,
	float particleSpawnInterval, float inflict_dmg_cd_time
) {
	auto it = effects_UID.emplace(uid);
	std::string_view uid_view = *(it.first);
	effects_UID_to_ID.emplace(uid_view, effects_data.size());
	effects_data.emplace_back(type, uid_view, stat, ui_sprite_id, effect_value, particle_id, particleSpawnInterval, inflict_dmg_cd_time);
}

uint32_t Effects::EffectsManager::get_effect_id(std::string_view uid) {
	return effects_UID_to_ID[uid];
}

Effects::EffectData& Effects::EffectsManager::get_effect_info(uint32_t id) {
	return effects_data[id];
}