#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>
#include <glm/ext/vector_float3.hpp>

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
	void add_light(std::string uid, float radius, glm::vec3 color);
	std::optional<uint32_t> get_light_id(std::string light_UID);
	LightComponent& get_light(uint32_t id);
private:
	ComponentsManager(){}
	~ComponentsManager(){}
	std::vector<LightComponent> appliable_lights;
	std::unordered_map<std::string/*UID*/, uint32_t/*ID*/> lights_uid_to_id;
};