#pragma once
#include <Entities/Particle.h>
#include <string>
#include <vector>
#include <unordered_set>

namespace Effects {
	enum EffectType : uint8_t {
		isHealSickness		= 0,
		isBuff				= 1,
		isDebuff			= 2,
		isDamagingDebuff	= 3,
		isUpgrade			= 4,
		isHealing			= 5
	};
	enum StatType : uint8_t {
		isTypeless	= 0,
		isHP		= 1,
		isDef		= 2,
		isMana		= 3,
		isSpeed		= 4,
		isRegen		= 5
	};

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
		EffectData(EffectType type, std::string_view uid, StatType stat, uint32_t ui_sprite_id, float effect_value,
			uint32_t particle_id = 0, float particle_spawn_cd = 0.0f, float dmg_cd = 0.0f) :
			effect_type{ type }, uid{ uid }, stat_type{ stat }, sprite_id{ ui_sprite_id }, effect_value{ effect_value },
			particle_id{ particle_id }, particle_spawn_cd{ particle_spawn_cd }, dmg_cd{ dmg_cd }
		{
			size_t pos = uid.rfind(':');
			if (pos == std::string_view::npos) name = uid; //if ':' not found, but this shouldn't happen if resource file is correct
			else name = uid.substr(pos + 1);
		}

		uint32_t particle_id = 0; //0 means effect has no particles
		uint32_t sprite_id = 0; //sprite that is used in player's UI
		float particle_spawn_cd = 0.0f; //time between every particle spawned
		float dmg_cd = 0.0f; //cd for a damaging debuff
		float effect_value = 0.0f; //value of effect that is applied to HP, DEF, Mana, Damage, etc.
		StatType stat_type = StatType::isTypeless;
		EffectType effect_type = EffectType::isBuff;
		std::string_view uid, name;
	};

	class EffectsManager {
	public:
		static EffectsManager* get_instance() {
			static EffectsManager effectsMgr;
			return &effectsMgr;
		}
		void init(uint32_t effects_size);
		void add_effect_info(EffectType type, std::string uid, StatType stat,
			float effect_value, uint32_t ui_sprite_id, uint32_t particle_id = 0,
			float particleSpawnInterval = 0.0f, float inflict_dmg_cd_time = 0.0f
		);
		uint32_t get_effect_id(std::string_view uid);
		EffectData& get_effect_info(uint32_t id);
	private:
		EffectsManager() {}
		~EffectsManager() {}
		std::unordered_set<std::string> effects_UID;
		std::unordered_map<std::string_view, uint32_t> effects_UID_to_ID;
		std::vector<EffectData> effects_data;
	};

}