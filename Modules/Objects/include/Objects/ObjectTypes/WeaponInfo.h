#pragma once
#include <Objects/GameObjects.h>

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
	WeaponInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}

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

class WeaponInfoFactory : public ObjectInfoFactory<WeaponInfo, ObjectType::isWeapon> {
public:
	void add_object(DataNode& data) override {
		ObjectInfo* info = create_object_info(data);
		try {
			uint8_t sub_type_id = data["sub_type"].get_as<uint8_t>();
			uint32_t damage = data["dmg"].get_as<uint32_t>();
			float crit_chance = data["crit_chance"].get_as<float>();
			float speed = data["speed"].get_as<float>();
			bool stackable = data["stackable"].get_as<bool>();
			float size_x = data["size_x"].get_as<float>();
			float size_y = data["size_y"].get_as<float>();
			float power = 0.0f, range = 0.0f;
			uint32_t use_mana_cost = 0;
			if (data.has_child("power")) power = data["power"].get_as<float>();
			if (data.has_child("range")) range = data["range"].get_as<float>();
			if (data.has_child("use_mana_cost")) use_mana_cost = data["use_mana_cost"].get_as<uint32_t>();

			static_cast<WeaponInfo*>(info)->weapon_type = WeaponType(sub_type_id);
			static_cast<WeaponInfo*>(info)->damage = damage;
			static_cast<WeaponInfo*>(info)->crit_chance = crit_chance;
			static_cast<WeaponInfo*>(info)->speed = speed;
			static_cast<WeaponInfo*>(info)->stackable = stackable;
			static_cast<WeaponInfo*>(info)->size_x = size_x;
			static_cast<WeaponInfo*>(info)->size_y = size_y;
			static_cast<WeaponInfo*>(info)->power = power;
			static_cast<WeaponInfo*>(info)->range = range;
			static_cast<WeaponInfo*>(info)->use_mana_cost = use_mana_cost;
		}
		catch (const std::exception& e) {
			GameObjects::throw_resource_error(info->uid, e.what());
		}

		/*auto& sounds_v = data["sounds"].array;
		uint8_t size = sounds_v.size();
		static_cast<WeaponInfo*>(info)->sounds = DynamicArray<uint16_t>(size);
		for (uint8_t i = 0; i < size; i++)
			static_cast<WeaponInfo*>(info)->sounds[i] = sounds_v[i].get_as<uint16_t>();*/
	}
};