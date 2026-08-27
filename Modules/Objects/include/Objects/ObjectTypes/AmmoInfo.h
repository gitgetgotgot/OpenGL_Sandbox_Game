#pragma once
#include <Objects/GameObjects.h>

enum AmmoType : uint8_t {
	isArrow = 0, isBullet = 1
};

class AmmoInfo : public ObjectInfo {
public:
	AmmoInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}

	AmmoType ammo_type;
	uint32_t entity_id;
	uint32_t damage;
};

class AmmoInfoFactory : public ObjectInfoFactory<AmmoInfo, ObjectType::isAmmo> {
public:
	void add_object(DataNode& data) override {
		ObjectInfo* info = create_object_info(data);
		try {
			uint8_t sub_type_id = data["sub_type"].get_as<uint8_t>();
			uint32_t damage = data["dmg"].get_as<uint32_t>();

			static_cast<AmmoInfo*>(info)->ammo_type = AmmoType(sub_type_id);
			static_cast<AmmoInfo*>(info)->damage = damage;
		}
		catch (const std::exception& e) {
			GameObjects::throw_resource_error(info->uid, e.what());
		}
	}
};