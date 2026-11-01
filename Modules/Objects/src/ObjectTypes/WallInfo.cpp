#include "Objects/ObjectTypes/WallInfo.h"
#include <Resources/ResourceErrors.h>

void Engine::Object::WallInfo::fill_data(const nlohmann::json& data) {
	ObjectInfo::fill_data(data);
	try {
		float toughness = data["toughness"].get<float>();
		this->toughness = toughness;
	}
	catch (const std::exception& e) {
		Engine::Resource::throw_resource_error(uid, e.what());
	}
}