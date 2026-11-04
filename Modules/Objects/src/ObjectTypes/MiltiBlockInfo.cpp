#include "Objects/ObjectTypes/MultiBlockInfo.h"

void Engine::Object::MultiBlockInfo::fill_data(const nlohmann::json& data) {
	ObjectInfo::fill_data(data);
	try {
		float tiles_width = data["width"].get<float>();
		float tiles_height = data["height"].get<float>();
		float toughness = data["toughness"].get<float>();
		bool collision = false, platform_collision = false;

		if (data.contains("collision"))
			collision = data["collision"].get<bool>();
		if (data.contains("platform_collision"))
			platform_collision = data["platform_collision"].get<bool>();

		this->tiles_width = tiles_width;
		this->tiles_height = tiles_height;
		this->toughness = toughness;
		this->collision = collision;
		this->platform_collision = platform_collision;
	}
	catch (const std::exception& e) {
		//Engine::Resource::throw_resource_error(uid, e.what());
	}
}