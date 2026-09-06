#include "Objects/ObjectTypes/WeaponInfo.h"
#include <Utility/ResourceErrors.h>

void CoreObject::WeaponInfo::fill_data(const DataNode& data) {
	ObjectInfo::fill_data(data);
	try {
		uint32_t damage = data["dmg"].get_as<uint32_t>();
		float crit_chance = data["crit_chance"].get_as<float>();
		float speed = data["speed"].get_as<float>();
		bool stackable = data["stackable"].get_as<bool>();
		float size_x = data["size_x"].get_as<float>();
		float size_y = data["size_y"].get_as<float>();
		float power = 0.0f, range = 0.0f;
		uint32_t use_mana_cost = 0;
		if (data.has_child("power")) power = data["power"].get_as<float>();
		if (data.has_child("range")) range = data["range"].get_as<float>();
		if (data.has_child("use_mana_cost")) use_mana_cost = data["use_mana_cost"].get_as<uint32_t>();

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
		ResourceLoading::throw_resource_error(uid, e.what());
	}
	/*auto& sounds_v = data["sounds"].array;
	uint8_t size = sounds_v.size();
	static_cast<WeaponInfo*>(info)->sounds = DynamicArray<uint16_t>(size);
	for (uint8_t i = 0; i < size; i++)
		static_cast<WeaponInfo*>(info)->sounds[i] = sounds_v[i].get_as<uint16_t>();*/
}