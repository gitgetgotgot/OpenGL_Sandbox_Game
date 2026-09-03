#include "Objects/ObjectTypes/AmmoInfo.h"
#include <Utility/ResourceErrors.h>

void CoreObject::AmmoInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		uint8_t sub_type_id = data["sub_type"].get_as<uint8_t>();
		uint32_t damage = data["dmg"].get_as<uint32_t>();
		this->ammo_type = AmmoType(sub_type_id);
		this->damage = damage;
	}
	catch (const std::exception& e) {
		ResourceLoading::throw_resource_error(uid, e.what());
	}
}