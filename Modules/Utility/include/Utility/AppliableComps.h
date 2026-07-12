#pragma once
#include <cstdint>

//structure that contains info about appliable effect
struct EffectComponent {
	float duration = 0.f;
	uint32_t id = 0;
	EffectComponent() {}
	EffectComponent(uint32_t id, float duration) : id{ id }, duration{ duration } {}
};

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
	void add_effect_component(float duration, uint32_t effect_id) {
		appliable_effects.emplace_back(effect_id, duration);
	}
	EffectComponent& get_effect_comp(uint32_t id) {
		return appliable_effects[id];
	}
	void add_light_component(float radius, glm::vec3 color) {
		appliable_lights.emplace_back(radius, color);
	}
	LightComponent& get_light_comp(uint32_t id) {
		return appliable_lights[id];
	}
private:
	ComponentsManager(){}
	~ComponentsManager(){}
	std::vector<EffectComponent> appliable_effects;
	std::vector<LightComponent> appliable_lights;
};