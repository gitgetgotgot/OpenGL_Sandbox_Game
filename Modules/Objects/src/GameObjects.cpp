#include <Objects/GameObjects.h>
#include <Utility/Sprite.h>
#include <Utility/AppliableComps.h>
#include <Utility/Effects.h>

//base method that fills base data (sprites, effects, lights), except dependencies (drops)
void GameObjects::fill_base_data(DataNode& data, ObjectInfo* info) {
	auto& sprites_v = data["sprites"].array;
	uint32_t size = sprites_v.size();
	info->sprites = DynamicArray<uint32_t>(size);
	for (uint32_t i = 0; i < size; i++) {
		std::string sprite_UID = sprites_v[i].get_as<std::string>();
		auto sprite_global_id = SpriteManager::get_instance()->get_sprite_id(sprite_UID);
		if (!sprite_global_id.has_value())
			throw_reference_error(std::string(info->uid), sprite_UID);
		info->sprites[i] = sprite_global_id.value();
	}
	if (data.has_child("effects")) {
		auto& effects_v = data["effects"].array;
		uint32_t effects_size = effects_v.size();
		info->effects = DynamicArray<EffectEntry>(effects_size);
		for (uint32_t i = 0; i < size; i++) {
			std::string effect_UID = effects_v[i]["effect_UID"].get_as<std::string>();
			auto effect_id_opt = Effects::EffectsManager::get_instance()->get_effect_id(effect_UID);
			if (!effect_id_opt.has_value())
				throw_reference_error(std::string(info->uid), effect_UID);
			float duration = effects_v[i]["duration"].get_as<float>();
			info->effects[i] = EffectEntry{ (uint16_t)effect_id_opt.value(), duration };
		}
	}
	if (data.has_child("lights")) {
		auto& lights_v = data["lights"].array;
		uint32_t lights_size = lights_v.size();
		info->lights = DynamicArray<uint16_t>(lights_size);
		for (uint32_t i = 0; i < size; i++) {
			std::string light_UID = lights_v[i].get_as<std::string>();
			auto light_id_opt = ComponentsManager::get_instance()->get_light_id(light_UID);
			if (!light_id_opt.has_value())
				throw_reference_error(std::string(info->uid), light_UID);
			info->lights[i] = light_id_opt.value();
		}
	}
}

void GameObjects::throw_reference_error(std::string_view item_uid, std::string_view resource_uid) {
	std::string msg;
	msg.reserve(item_uid.size() + resource_uid.size() + 64);
	msg += "Load error: <";
	msg += item_uid;
	msg += ">:\n	Resource <";
	msg += resource_uid;
	msg += "> not found!";
	throw std::runtime_error(msg);
}

void GameObjects::throw_resource_error(std::string_view item_uid, std::string_view msg) {
	std::string message;
	message.reserve(item_uid.size() + msg.size() + 64);
	message += "Load error: <";
	message += item_uid;
	message += ">: Data is corrupted or incorrect:";
	message += msg;
	throw std::runtime_error(message);
}