#include "Objects/ObjectTypes/WeaponInfo.h"
#include <Resources/ResourceErrors.h>

void Engine::Object::WeaponInfo::fill_data(const nlohmann::json& data) {
	ObjectInfo::fill_data(data);
	try {
		uint32_t damage = data["dmg"].get<uint32_t>();
		float crit_chance = data["crit_chance"].get<float>();
		float speed = data["speed"].get<float>();
		bool stackable = data["stackable"].get<bool>();
		float size_x = data["size_x"].get<float>();
		float size_y = data["size_y"].get<float>();
		float power = 0.0f, range = 0.0f;
		uint32_t use_mana_cost = 0;
		if (data.contains("power")) power = data["power"].get<float>();
		if (data.contains("range")) range = data["range"].get<float>();
		if (data.contains("use_mana_cost")) use_mana_cost = data["use_mana_cost"].get<uint32_t>();

		this->damage = damage;
		this->crit_chance = crit_chance;
		this->speed = speed;
		this->stackable = stackable;
		this->size_x = size_x;
		this->size_y = size_y;
		this->power = power;
		this->range = range;
		this->use_mana_cost = use_mana_cost;
	}
	catch (const std::exception& e) {
		Engine::Resource::throw_resource_error(uid, e.what());
	}
	/*auto& sounds_v = data["sounds"].array;
	uint8_t size = sounds_v.size();
	static_cast<WeaponInfo*>(info)->sounds = DynamicArray<uint16_t>(size);
	for (uint8_t i = 0; i < size; i++)
		static_cast<WeaponInfo*>(info)->sounds[i] = sounds_v[i].get_as<uint16_t>();*/
}