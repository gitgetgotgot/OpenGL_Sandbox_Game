#include "Objects/ObjectInfo.h"
#include "Objects/ObjectManager.h"
#include <Resources/Sprite.h>
#include <Resources/Light.h>
#include <Resources/Effect.h>
#include <Resources/TagRegistry.h>

Engine::Object::ObjectInfo::ObjectInfo(ObjectType type) : objectType{ type } {}

void Engine::Object::ObjectInfo::_set_UID(std::string_view uid) {
	this->uid = uid;
	size_t pos = uid.rfind(':');
	this->name = uid.substr(pos + 1);
}

bool Engine::Object::ObjectInfo::_has_tag(const uint16_t& TAG) const {
	return std::binary_search(tags.data, tags.data + tags.size, TAG);
}

void Engine::Object::ObjectInfo::_sort_tags() const {
	std::sort(tags.data, tags.data + tags.size);
}

void Engine::Object::ObjectInfo::fill_data(const nlohmann::json& data) {
	const auto& sprites_v = data["sprites"].get<std::vector<nlohmann::json>>();
	uint32_t size = sprites_v.size();
	this->sprites = Engine::Util::DynamicArray<uint32_t>(size);

	for (uint32_t i = 0; i < size; i++) {
		std::string sprite_UID = sprites_v[i].get<std::string>();
		auto sprite_global_id = Engine::Resource::SpriteManager::Instance().get_sprite_id(sprite_UID);

		if (sprite_global_id.has_value())
			this->sprites[i] = sprite_global_id.value();
		else {
			//Engine::Resource::throw_reference_error(std::string(this->uid), sprite_UID);
		}
	}
	if (data.contains("effects")) {
		const auto& effects_v = data["effects"].get<std::vector<nlohmann::json>>();
		uint32_t effects_size = effects_v.size();
		this->effects = Engine::Util::DynamicArray<Engine::Resource::EffectEntry>(effects_size);

		for (uint32_t i = 0; i < effects_size; i++) {
			std::string effect_UID = effects_v[i]["effect_UID"].get<std::string>();
			float duration = effects_v[i]["duration"].get<float>();
			auto effect_id_opt = Engine::Resource::EffectsManager::Instance().get_effect_id(effect_UID);

			if (effect_id_opt.has_value())
				this->effects[i] = Engine::Resource::EffectEntry{ (uint16_t)effect_id_opt.value(), duration };
			else {
				//Engine::Resource::throw_reference_error(std::string(this->uid), effect_UID);
			}
		}
	}
	if (data.contains("lights")) {
		const auto& lights_v = data["lights"].get<std::vector<nlohmann::json>>();
		uint32_t lights_size = lights_v.size();
		this->lights = Engine::Util::DynamicArray<uint16_t>(lights_size);

		for (uint32_t i = 0; i < lights_size; i++) {
			std::string light_UID = lights_v[i].get<std::string>();
			auto light_id_opt = Engine::Resource::LightSourceManager::Instance().get_light_id(light_UID);

			if (light_id_opt.has_value())
				this->lights[i] = light_id_opt.value();
			else {
				//Engine::Resource::throw_reference_error(std::string(this->uid), light_UID);
			}
		}
	}
	if (data.contains("tags")) {
		const auto& tags_v = data["tags"].get<std::vector<nlohmann::json>>();
		uint32_t tags_size = tags_v.size();
		this->tags = Engine::Util::DynamicArray<uint16_t>(tags_size);

		for (uint32_t i = 0; i < tags_size; i++) {
			std::string tag_UID = tags_v[i].get<std::string>();
			auto tag_id_opt = Engine::Resource::Tag::TagRegistry::Instance().get_tag(tag_UID);

			if (tag_id_opt.has_value())
				this->tags[i] = tag_id_opt.value();
			else {
				//Engine::Resource::throw_reference_error(std::string(this->uid), tag_UID);
			}
		}
		_sort_tags();
	}
}

void Engine::Object::ObjectInfo::fill_dependent_data(const nlohmann::json& data) {
	if (data.contains("drops")) {
		const auto& drops_v = data["drops"].get<std::vector<nlohmann::json>>();
		uint32_t size = drops_v.size();
		this->drops = Engine::Util::DynamicArray<Engine::Resource::DropEntry>(size);

		for (uint32_t i = 0; i < size; i++) {
			std::string drop_UID = drops_v[i]["UID"].get<std::string>();
			uint16_t drop_Amount = drops_v[i]["Amount"].get<uint16_t>();
			auto drop_global_id = ObjectManager::Instance().get_object_id(drop_UID);

			if (drop_global_id.has_value()) {
				this->drops[i].id = drop_global_id.value();
				this->drops[i].amount = drop_Amount;
			}
			else {
				//Engine::Resource::throw_reference_error(std::string(this->uid), drop_UID);
			}
		}
	}
}
