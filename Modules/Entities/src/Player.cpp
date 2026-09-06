#include <Entities/Player.h>

void PlayerStats::updateEffects(float deltaTime) {
	for (int i = 0; i < effects.size(); i++) {
		CoreResource::Effect& effect = effects[i];
		CoreResource::EffectData& effectData = CoreResource::EffectsManager::get_instance().get_effect_info(effect.id);

		if (effect.updateEffect(deltaTime)) {
			if (effectData.effect_type == CoreResource::EffectType::isHealSickness) {
				hasPotionSickness = false;
			}
			else if (effectData.effect_type == CoreResource::EffectType::isBuff) {
				manage_effect(effect, false); //remove effect
			}
			else if (effectData.effect_type == CoreResource::EffectType::isDebuff) {
				manage_effect(effect, false); //remove effect
			}
			effects.erase(effects.begin() + i);
			i--;
			continue;
		}

		if (effectData.effect_type == CoreResource::EffectType::isDamagingDebuff)
			if (effect.delta_dmg_time >= effectData.dmg_cd) {
				effect.delta_dmg_time = 0.0f;
				inflictDamage(effectData.effect_value);
			}

		if (effectData.particle_id != 0 && effect.currentParticleTime >= effectData.particle_spawn_cd) {
			effect.currentParticleTime = 0.0f;
			//effectData.emit_particle();
		}
	}
}

void PlayerStats::manage_effect(CoreResource::Effect& effect, bool apply) {
	CoreResource::EffectData& effectData = CoreResource::EffectsManager::get_instance().get_effect_info(effect.id);
	if (effectData.effect_type == CoreResource::EffectType::isBuff) {
		switch (effectData.stat_type) {
		case CoreResource::EffectStatType::isHP: {
			if (apply) currentHP += effectData.effect_value;
			else currentHP -= effectData.effect_value;
			break;
		}
		case CoreResource::EffectStatType::isDef: {
			if (apply) DEF += effectData.effect_value;
			else DEF -= effectData.effect_value;
			break;
		}
		case CoreResource::EffectStatType::isMana: {
			if (apply) MANA += effectData.effect_value;
			else MANA -= effectData.effect_value;
			break;
		}
		case CoreResource::EffectStatType::isRegen: {
			if (apply) regeneration += effectData.effect_value;
			else regeneration -= effectData.effect_value;
			break;
		}
		case CoreResource::EffectStatType::isSpeed: {
			if (apply) speedFactor += effectData.effect_value;
			else speedFactor -= effectData.effect_value;
			break;
		}
		}
	}
	else if (effectData.effect_type == CoreResource::EffectType::isDebuff) {
		switch (effectData.stat_type) {
		case CoreResource::EffectStatType::isHP: {

		}
		case CoreResource::EffectStatType::isDef: {

		}
		case CoreResource::EffectStatType::isMana: {

		}
		case CoreResource::EffectStatType::isRegen: {

		}
		case CoreResource::EffectStatType::isSpeed: {

		}
		}
	}
	else if (effectData.effect_type == CoreResource::EffectType::isHealing) {
		currentHP += effectData.effect_value;
		if (currentHP > HP) currentHP = HP;
	}
	else if (effectData.effect_type == CoreResource::EffectType::isHealSickness) {
		hasPotionSickness = true;
	}
}
