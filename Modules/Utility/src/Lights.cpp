#include <Utility/Lights.h>

void CoreResource::LightsManager::ClearData() {
	lights = {};
	light_UID_to_ID = {};
}

void CoreResource::LightsManager::add_light(std::string uid, float radius, glm::vec3 color) {
	light_UID_to_ID.emplace(uid, lights.size());
	lights.emplace_back(radius, color);
}

std::optional<uint32_t> CoreResource::LightsManager::get_light_id(std::string light_UID) {
	auto it = light_UID_to_ID.find(light_UID);
	if (it != light_UID_to_ID.end())
		return it->second;
	else
		return std::nullopt;
}

CoreResource::LightComponent& CoreResource::LightsManager::get_light(uint32_t id) {
	return lights[id];
}