#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	enum WeaponType : uint8_t {
		isPickaxe = 0, isSword = 1, isPiercing = 2, isAxe = 3, isHammer = 4, isGun = 5, isShotgun = 6, isBow = 7, isThrowable = 8,
		isSpear = 9, isMagical = 10
	};

	struct ProjectileEntry {
		uint16_t id = 0;
		float cooldown = 0.0f;
	};

	class WeaponInfo : public ObjectInfo {
	public:
		WeaponInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		WeaponType weapon_type = WeaponType::isPickaxe;
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