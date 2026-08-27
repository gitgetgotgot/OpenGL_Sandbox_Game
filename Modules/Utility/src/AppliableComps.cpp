#include <Utility/AppliableComps.h>

void ComponentsManager::add_light(std::string uid, float radius, glm::vec3 color) {
	lights_uid_to_id.emplace(uid, appliable_lights.size());
	appliable_lights.emplace_back(radius, color);
}

std::optional<uint32_t> ComponentsManager::get_light_id(std::string light_UID) {
	auto it = lights_uid_to_id.find(light_UID);
	if (it != lights_uid_to_id.end())
		return it->second;
	else
		return std::nullopt;
}

LightComponent& ComponentsManager::get_light(uint32_t id) {
	return appliable_lights[id];
}