#include "Objects/ObjectTypes/BlockInfo.h"
#include <Utility/ResourceErrors.h>

void CoreObject::BlockInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		float toughness = data["toughness"].get_as<float>();
		bool collision = false, platform_collision = false;

		if (data.has_child("collision"))
			collision = data["collision"].get_as<bool>();
		if (data.has_child("platform_collision"))
			platform_collision = data["platform_collision"].get_as<bool>();

		this->toughness = toughness;
		this->collision = collision;
		this->platform_collision = platform_collision;
	}
	catch (const std::exception& e) {
		ResourceLoading::throw_resource_error(uid, e.what());
	}
}