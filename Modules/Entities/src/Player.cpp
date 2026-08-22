#include <Entities/Player.h>

void PlayerStats::updateEffects(float deltaTime) {
	for (int i = 0; i < effects.size(); i++) {
		Effects::Effect& effect = effects[i];
		Effects::EffectData& effectData = Effects::EffectsManager::get_instance()->get_effect_info(effect.id);

		if (effect.updateEffect(deltaTime)) {
			if (effectData.effect_type == Effects::EffectType::isHealSickness) {
				hasPotionSickness = false;
			}
			else if (effectData.effect_type == Effects::EffectType::isBuff) {
				manage_effect(effect, false); //remove effect
			}
			else if (effectData.effect_type == Effects::EffectType::isDebuff) {
				manage_effect(effect, false); //remove effect
			}
			effects.erase(effects.begin() + i);
			i--;
			continue;
		}

		if (effectData.effect_type == Effects::EffectType::isDamagingDebuff)
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

void PlayerStats::manage_effect(Effects::Effect& effect, bool apply) {
	Effects::EffectData& effectData = Effects::EffectsManager::get_instance()->get_effect_info(effect.id);
	if (effectData.effect_type == Effects::EffectType::isBuff) {
		switch (effectData.stat_type) {
		case Effects::StatType::isHP: {
			if (apply) currentHP += effectData.effect_value;
			else currentHP -= effectData.effect_value;
			break;
		}
		case Effects::StatType::isDef: {
			if (apply) DEF += effectData.effect_value;
			else DEF -= effectData.effect_value;
			break;
		}
		case Effects::StatType::isMana: {
			if (apply) MANA += effectData.effect_value;
			else MANA -= effectData.effect_value;
			break;
		}
		case Effects::StatType::isRegen: {
			if (apply) regeneration += effectData.effect_value;
			else regeneration -= effectData.effect_value;
			break;
		}
		case Effects::StatType::isSpeed: {
			if (apply) speedFactor += effectData.effect_value;
			else speedFactor -= effectData.effect_value;
			break;
		}
		}
	}
	else if (effectData.effect_type == Effects::EffectType::isDebuff) {
		switch (effectData.stat_type) {
		case Effects::StatType::isHP: {

		}
		case Effects::StatType::isDef: {

		}
		case Effects::StatType::isMana: {

		}
		case Effects::StatType::isRegen: {

		}
		case Effects::StatType::isSpeed: {

		}
		}
	}
	else if (effectData.effect_type == Effects::EffectType::isHealing) {
		currentHP += effectData.effect_value;
		if (currentHP > HP) currentHP = HP;
	}
	else if (effectData.effect_type == Effects::EffectType::isHealSickness) {
		hasPotionSickness = true;
	}
}
