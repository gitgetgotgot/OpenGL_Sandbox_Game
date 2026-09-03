#include "Objects/ObjectTypes/MultiBlockInfo.h"
#include <Utility/ResourceErrors.h>

void CoreObject::MultiBlockInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		uint8_t sub_type_id = data["sub_type"].get_as<uint8_t>();
		float tiles_width = data["width"].get_as<float>();
		float tiles_height = data["height"].get_as<float>();
		float toughness = data["toughness"].get_as<float>();
		bool collision = false, platform_collision = false;

		if (data.has_child("collision"))
			collision = data["collision"].get_as<bool>();
		if (data.has_child("platform_collision"))
			platform_collision = data["platform_collision"].get_as<bool>();

		this->multi_block_type = MultiBlockType(sub_type_id);
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