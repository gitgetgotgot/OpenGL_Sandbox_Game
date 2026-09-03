#include "Objects/ObjectTypes/WallInfo.h"
#include <Utility/ResourceErrors.h>

void CoreObject::WallInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		float toughness = data["toughness"].get_as<float>();
		this->toughness = toughness;
	}
	catch (const std::exception& e) {
		ResourceLoading::throw_resource_error(uid, e.what());
	}
}