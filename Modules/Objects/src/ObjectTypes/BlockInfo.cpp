#include "Objects/ObjectTypes/BlockInfo.h"
#include <Resources/ResourceErrors.h>

void Engine::Object::BlockInfo::fill_data(const nlohmann::json& data) {
	ObjectInfo::fill_data(data);
	try {
		float toughness = data["toughness"].get<float>();
		bool collision = false, platform_collision = false;

		if (data.contains("collision"))
			collision = data["collision"].get<bool>();
		if (data.contains("platform_collision"))
			platform_collision = data["platform_collision"].get<bool>();

		this->toughness = toughness;
		this->collision = collision;
		this->platform_collision = platform_collision;
	}
	catch (const std::exception& e) {
		Engine::Resource::throw_resource_error(uid, e.what());
	}
}