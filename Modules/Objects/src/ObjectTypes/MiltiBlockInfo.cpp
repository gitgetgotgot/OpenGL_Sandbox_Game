#include "Objects/ObjectTypes/MultiBlockInfo.h"
#include <Resources/ResourceErrors.h>

void CoreObject::MultiBlockInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		float tiles_width = data["width"].get_as<float>();
		float tiles_height = data["height"].get_as<float>();
		float toughness = data["toughness"].get_as<float>();
		bool collision = false, platform_collision = false;

		if (data.has_child("collision"))
			collision = data["collision"].get_as<bool>();
		if (data.has_child("platform_collision"))
			platform_collision = data["platform_collision"].get_as<bool>();

		this->tiles_width = tiles_width;
		this->tiles_height = tiles_height;
		this->toughness = toughness;
		this->collision = collision;
		this->platform_collision = platform_collision;
	}
	catch (const std::exception& e) {
		ResourceLoading::throw_resource_error(uid, e.what());
	}
}