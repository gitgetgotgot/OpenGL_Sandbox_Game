#include "Resources/Light.h"

void Engine::Resource::LightSourceManager::setup() {
	//add empty light with global_ID = 0
	lights.emplace_back();
	light_UID_to_ID["Core:Empty"] = 0;
}

void Engine::Resource::LightSourceManager::ClearData() {
	lights = {};
	light_UID_to_ID = {};
}

void Engine::Resource::LightSourceManager::add_light(std::string uid, float radius, glm::vec3 color) {
	light_UID_to_ID.emplace(uid, lights.size());
	lights.emplace_back(radius, color);
}

std::optional<uint32_t> Engine::Resource::LightSourceManager::get_light_id(std::string light_UID) {
	auto it = light_UID_to_ID.find(light_UID);
	if (it != light_UID_to_ID.end())
		return it->second;
	else
		return std::nullopt;
}

Engine::Resource::LightSource& Engine::Resource::LightSourceManager::get_light(uint32_t id) {
	return lights[id];
}