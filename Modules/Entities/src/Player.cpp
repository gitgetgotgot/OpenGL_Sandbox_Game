#include <Entities/Player.h>

void PlayerStats::updateEffects(float deltaTime) {
	for (int i = 0; i < effects.size(); i++) {
		Engine::Resource::Effect& effect = effects[i];
		Engine::Resource::EffectData& effectData = Engine::Resource::EffectsManager::Instance().get_effect_info(effect.id);

		if (effect.updateEffect(deltaTime)) {
			if (effectData.effect_type == Engine::Resource::Effect::EffectType::isHealSickness) {
				hasPotionSickness = false;
			}
			else if (effectData.effect_type == Engine::Resource::Effect::EffectType::isBuff) {
				manage_effect(effect, false); //remove effect
			}
			else if (effectData.effect_type == Engine::Resource::Effect::EffectType::isDebuff) {
				manage_effect(effect, false); //remove effect
			}
			effects.erase(effects.begin() + i);
			i--;
			continue;
		}

		if (effectData.effect_type == Engine::Resource::Effect::EffectType::isDamagingDebuff)
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

void PlayerStats::manage_effect(Engine::Resource::Effect& effect, bool apply) {
	Engine::Resource::EffectData& effectData = Engine::Resource::EffectsManager::Instance().get_effect_info(effect.id);
	if (effectData.effect_type == Engine::Resource::Effect::EffectType::isBuff) {
		switch (effectData.stat_type) {
		case Engine::Resource::Effect::EffectStatType::isHP: {
			if (apply) currentHP += effectData.effect_value;
			else currentHP -= effectData.effect_value;
			break;
		}
		case Engine::Resource::Effect::EffectStatType::isDef: {
			if (apply) DEF += effectData.effect_value;
			else DEF -= effectData.effect_value;
			break;
		}
		case Engine::Resource::Effect::EffectStatType::isMana: {
			if (apply) MANA += effectData.effect_value;
			else MANA -= effectData.effect_value;
			break;
		}
		case Engine::Resource::Effect::EffectStatType::isRegen: {
			if (apply) regeneration += effectData.effect_value;
			else regeneration -= effectData.effect_value;
			break;
		}
		case Engine::Resource::Effect::EffectStatType::isSpeed: {
			if (apply) speedFactor += effectData.effect_value;
			else speedFactor -= effectData.effect_value;
			break;
		}
		}
	}
	else if (effectData.effect_type == Engine::Resource::Effect::EffectType::isDebuff) {
		switch (effectData.stat_type) {
		case Engine::Resource::Effect::EffectStatType::isHP: {

		}
		case Engine::Resource::Effect::EffectStatType::isDef: {

		}
		case Engine::Resource::Effect::EffectStatType::isMana: {

		}
		case Engine::Resource::Effect::EffectStatType::isRegen: {

		}
		case Engine::Resource::Effect::EffectStatType::isSpeed: {

		}
		}
	}
	else if (effectData.effect_type == Engine::Resource::Effect::EffectType::isHealing) {
		currentHP += effectData.effect_value;
		if (currentHP > HP) currentHP = HP;
	}
	else if (effectData.effect_type == Engine::Resource::Effect::EffectType::isHealSickness) {
		hasPotionSickness = true;
	}
}
