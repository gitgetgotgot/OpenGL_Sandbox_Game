#pragma once
#include <cstdint>
#include <unordered_map>

//structure that contains info about appliable lighting
struct LightComponent {
	glm::vec3 light_color{ 0.f, 0.f, 0.f };
	float light_radius = 0.f; //measured in blocks
	LightComponent() {}
	LightComponent(float radius, glm::vec3 color) : light_radius{ radius }, light_color{ color } {}
};

class ComponentsManager {
public:
	static ComponentsManager* get_instance() {
		static ComponentsManager compsMgr;
		return &compsMgr;
	}
	void add_light(std::string uid, float radius, glm::vec3 color) {
		lights_uid_to_id.emplace(uid, appliable_lights.size());
		appliable_lights.emplace_back(radius, color);
	}
	uint32_t get_light_id(std::string light_UID) {
		return lights_uid_to_id[light_UID];
	}
	LightComponent& get_light(uint32_t id) {
		return appliable_lights[id];
	}
private:
	ComponentsManager(){}
	~ComponentsManager(){}
	std::vector<LightComponent> appliable_lights;
	std::unordered_map<std::string/*UID*/, uint32_t/*ID*/> lights_uid_to_id;
};