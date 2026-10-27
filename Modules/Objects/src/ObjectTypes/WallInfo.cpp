#include "Objects/ObjectTypes/WallInfo.h"
#include <Resources/ResourceErrors.h>

void Engine::Object::WallInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		float toughness = data["toughness"].get_as<float>();
		this->toughness = toughness;
	}
	catch (const std::exception& e) {
		Engine::Resource::throw_resource_error(uid, e.what());
	}
}