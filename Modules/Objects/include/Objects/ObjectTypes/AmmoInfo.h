#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	enum AmmoType : uint8_t {
		isArrow = 0, isBullet = 1
	};

	class AmmoInfo : public ObjectInfo {
	public:
		AmmoInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		AmmoType ammo_type;
		uint32_t entity_id;
		uint32_t damage;
	};

	class AmmoInfoFactory : public ObjectInfoFactory<AmmoInfo, ObjectType::isAmmo> {};
}