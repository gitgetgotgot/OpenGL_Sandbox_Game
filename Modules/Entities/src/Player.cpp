#include <Entities/Player.h>

void PlayerStats::updateEffects(float deltaTime) {
	for (int i = 0; i < effects.size(); i++) {
		Effects::Effect& effect = effects[i];
		Effects::EffectData& effectData = Effects::EffectsManager::get_instance()->get_effect_info(effect.id);

		if (effect.updateEffect(deltaTime)) {
			if (effectData.type == Effects::EffectType::isHealSickness) {
				hasPotionSickness = false;
			}
			else if (effectData.type == Effects::EffectType::isBuff) {
				manage_effect(effect, false); //remove effect
			}
			else if (effectData.type == Effects::EffectType::isDebuff) {
				manage_effect(effect, false); //remove effect
			}
			effects.erase(effects.begin() + i);
			i--;
			continue;
		}

		if (effectData.type == Effects::EffectType::isDamagingDebuff)
			if (effect.delta_dmg_time >= effectData.inflict_dmg_cd_time) {
				effect.delta_dmg_time = 0.0f;
				inflictDamage(effectData.effectPower);
			}

		if (effectData.particle_id != 0 && effect.currentParticleTime >= effectData.particleSpawnInterval) {
			effect.currentParticleTime = 0.0f;
			//effectData.emit_particle();
		}
	}
}

void PlayerStats::manage_effect(Effects::Effect& effect, bool apply) {
	Effects::EffectData& effectData = Effects::EffectsManager::get_instance()->get_effect_info(effect.id);
	if (effectData.type == Effects::EffectType::isBuff) {
		switch (effectData.statName) {
		case Effects::StatName::isHP: {
			if (apply) currentHP += effectData.effectPower;
			else currentHP -= effectData.effectPower;
			break;
		}
		case Effects::StatName::isDef: {
			if (apply) DEF += effectData.effectPower;
			else DEF -= effectData.effectPower;
			break;
		}
		case Effects::StatName::isMana: {
			if (apply) MANA += effectData.effectPower;
			else MANA -= effectData.effectPower;
			break;
		}
		case Effects::StatName::isRegen: {
			if (apply) regeneration += effectData.effectPower;
			else regeneration -= effectData.effectPower;
			break;
		}
		case Effects::StatName::isSpeed: {
			if (apply) speedFactor += effectData.effectPower;
			else speedFactor -= effectData.effectPower;
			break;
		}
		}
	}
	else if (effectData.type == Effects::EffectType::isDebuff) {
		switch (effectData.statName) {
		case Effects::StatName::isHP: {

		}
		case Effects::StatName::isDef: {

		}
		case Effects::StatName::isMana: {

		}
		case Effects::StatName::isRegen: {

		}
		case Effects::StatName::isSpeed: {

		}
		}
	}
	else if (effectData.type == Effects::EffectType::isHealing) {
		currentHP += effectData.effectPower;
		if (currentHP > HP) currentHP = HP;
	}
	else if (effectData.type == Effects::EffectType::isHealSickness) {
		hasPotionSickness = true;
	}
}
