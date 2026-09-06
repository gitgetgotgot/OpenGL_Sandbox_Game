#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	struct ProjectileEntry {
		uint16_t id = 0;
		float cooldown = 0.0f;
	};

	class WeaponInfo : public ObjectInfo {
	public:
		WeaponInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		uint32_t damage = 1.0f;
		float crit_chance = 5.0f;
		float speed = 1.0f;
		bool stackable = false;
		float size_x = 1.0f;
		float size_y = 1.0f;
		DynamicArray<uint16_t> sounds;
		DynamicArray<ProjectileEntry> projectiles;
		//for instrumental weapons
		float power = 1.0f;
		float range = 1.0f;
		//for magic weapons
		uint32_t use_mana_cost = 0;
	};

	class WeaponInfoFactory : public ObjectInfoFactory<WeaponInfo, ObjectType::isWeapon> {};
}