#include "Objects/ObjectInfo.h"
#include "Objects/ObjectManager.h"
#include <Utility/Sprite.h>
#include <Utility/Lights.h>
#include <Utility/Effects.h>
#include <Utility/ResourceErrors.h>

CoreObject::ObjectInfo::ObjectInfo(ObjectType type) : objectType{ type } {}

void CoreObject::ObjectInfo::set_UID(std::string_view uid) {
	this->uid = uid;
	size_t pos = uid.rfind(':');
	this->name = uid.substr(pos + 1);
}

void CoreObject::ObjectInfo::fill_data(const DataNode& data) {
	auto& sprites_v = data["sprites"].array;
	uint32_t size = sprites_v.size();
	this->sprites = DynamicArray<uint32_t>(size);
	for (uint32_t i = 0; i < size; i++) {
		std::string sprite_UID = sprites_v[i].get_as<std::string>();
		auto sprite_global_id = CoreResource::SpriteManager::get_instance()->get_sprite_id(sprite_UID);
		if (sprite_global_id.has_value())
			this->sprites[i] = sprite_global_id.value();
		else
			ResourceLoading::throw_reference_error(std::string(this->uid), sprite_UID);
	}
	if (data.has_child("effects")) {
		auto& effects_v = data["effects"].array;
		uint32_t effects_size = effects_v.size();
		this->effects = DynamicArray<EffectEntry>(effects_size);
		for (uint32_t i = 0; i < size; i++) {
			std::string effect_UID = effects_v[i]["effect_UID"].get_as<std::string>();
			float duration = effects_v[i]["duration"].get_as<float>();
			auto effect_id_opt = CoreResource::EffectsManager::get_instance()->get_effect_id(effect_UID);
			if (effect_id_opt.has_value())
				this->effects[i] = EffectEntry{ (uint16_t)effect_id_opt.value(), duration };
			else
				ResourceLoading::throw_reference_error(std::string(this->uid), effect_UID);
		}
	}
	if (data.has_child("lights")) {
		auto& lights_v = data["lights"].array;
		uint32_t lights_size = lights_v.size();
		this->lights = DynamicArray<uint16_t>(lights_size);
		for (uint32_t i = 0; i < size; i++) {
			std::string light_UID = lights_v[i].get_as<std::string>();
			auto light_id_opt = CoreResource::LightsManager::get_instance()->get_light_id(light_UID);
			if (light_id_opt.has_value())
				this->lights[i] = light_id_opt.value();
			else
				ResourceLoading::throw_reference_error(std::string(this->uid), light_UID);
		}
	}
}

void CoreObject::ObjectInfo::fill_dependent_data(const DataNode& data) {
	if (data.has_child("drops")) {
		auto& drops_v = data["drops"].array;
		uint32_t size = drops_v.size();
		this->drops = DynamicArray<DropEntry>(size);
		for (uint32_t i = 0; i < size; i++) {
			std::string drop_UID = drops_v[i]["UID"].get_as<std::string>();
			uint16_t drop_Amount = drops_v[i]["Amount"].get_as<uint16_t>();
			auto drop_global_id = ObjectManager::get_instance()->get_object_id(drop_UID);
			if (drop_global_id.has_value()) {
				this->drops[i].id = drop_global_id.value();
				this->drops[i].amount = drop_Amount;
			}
			else
				ResourceLoading::throw_reference_error(std::string(this->uid), drop_UID);
		}
	}
}
