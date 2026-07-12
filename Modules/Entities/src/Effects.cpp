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

void Effects::EffectsManager::add_effect_info(EffectType type, std::string name, StatName stat,
	float effectPower, uint32_t particle_id, float particleSpawnInterval,
	float inflict_dmg_cd_time
) {
	effects_data.emplace_back(type, name, stat, effectPower, particle_id, particleSpawnInterval, inflict_dmg_cd_time);
}

Effects::EffectData& Effects::EffectsManager::get_effect_info(uint32_t id) {
	return effects_data[id];
}