#pragma once
#include <Entities/Particle.h>
#include <string>
#include <vector>

namespace Effects {
	enum EffectType : uint8_t { isBuff, isDebuff, isDamagingDebuff, isImmediateBuff, isHealing, isHealSickness };
	enum StatName : uint8_t { isHP, isDef, isMana, isSpeed, isRegen };

	struct Effect {
		float duration;

		float delta_dmg_time = 0.f; //delta time that is used for debuffs with damage

		float particleSpawnInterval = 0.1f;
		float currentParticleTime = 0.f;
		uint32_t id;

		Effect(float duration, uint32_t id) : duration{ duration }, id{ id } {}
		bool updateEffect(float deltaTime);
	};

	class EffectData {
	public:
		uint32_t particle_id = 0; //0 means that effect has no particles
		float particleSpawnInterval = 0.0f;
		float inflict_dmg_cd_time = 0.0f;
		float effectPower = 0.0f; //power of effect that is applied to HP, DEF, Mana, Damage, etc.
		StatName statName = StatName::isHP;
		EffectType type = EffectType::isBuff;
		std::string name;
		uint32_t sprite_id = 0; //sprite that is used in player's UI
		EffectData(EffectType type, std::string name, StatName stat, float effectPower,
			uint32_t particle_id = 0, float particleSpawnInterval = 0.0f, float inflict_dmg_cd_time = 0.0f) :
			type{ type }, name{ name }, statName{ stat }, effectPower{ effectPower }, particle_id{ particle_id },
			particleSpawnInterval{ particleSpawnInterval }, inflict_dmg_cd_time{ inflict_dmg_cd_time } {}
	};

	class EffectsManager {
	public:
		static EffectsManager* get_instance() {
			static EffectsManager effectsMgr;
			return &effectsMgr;
		}
		void init(uint32_t effects_size);
		void add_effect_info(EffectType type, std::string name, StatName stat,
			float effectPower, uint32_t particle_id = 0, float particleSpawnInterval = 0.0f,
			float inflict_dmg_cd_time = 0.0f
		);
		EffectData& get_effect_info(uint32_t id);
	private:
		EffectsManager() {}
		~EffectsManager() {}
		std::vector<EffectData> effects_data;
	};

}