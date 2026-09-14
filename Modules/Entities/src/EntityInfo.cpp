#include "Entities/EntityInfo.h"
#include "Entities/EntityFactory.h"
#include <Resources/Lights.h>
#include <Resources/TagRegistry.h>
#include <Resources/ResourceErrors.h>

CoreEntity::EntityInfo::EntityInfo(CoreEntity::EntityMainType type) : main_type{ type } {}

void CoreEntity::EntityInfo::_set_UID(std::string_view uid) {
	this->uid = uid;
	size_t pos = uid.rfind(':');
	this->name = uid.substr(pos + 1);
}

bool CoreEntity::EntityInfo::_has_tag(const uint16_t& TAG) const {
	return std::binary_search(tags.data, tags.data + tags.size, TAG);
}

void CoreEntity::EntityInfo::_sort_tags() const {
	std::sort(tags.data, tags.data + tags.size);
}

void CoreEntity::EntityInfo::fill_data(const DataNode& data) {
	if (data.has_child("lights")) {
		auto& lights_v = data["lights"].array;
		uint32_t lights_size = lights_v.size();
		this->lights = DynamicArray<uint16_t>(lights_size);
		for (uint32_t i = 0; i < lights_size; i++) {
			std::string light_UID = lights_v[i].get_as<std::string>();
			auto light_id_opt = CoreResource::LightsManager::get_instance().get_light_id(light_UID);
			if (light_id_opt.has_value())
				this->lights[i] = light_id_opt.value();
			else
				ResourceLoading::throw_reference_error(std::string(this->uid), light_UID);
		}
	}
	if (data.has_child("tags")) {
		auto& tags_v = data["tags"].array;
		uint32_t tags_size = tags_v.size();
		this->tags = DynamicArray<uint16_t>(tags_size);
		for (uint32_t i = 0; i < tags_size; i++) {
			std::string tag_UID = tags_v[i].get_as<std::string>();
			auto tag_id_opt = TagRegistry::get_instance().get_tag(tag_UID);
			if (tag_id_opt.has_value())
				this->tags[i] = tag_id_opt.value();
			else
				ResourceLoading::throw_reference_error(std::string(this->uid), tag_UID);
		}
		_sort_tags();
	}
	std::string entity_class_UID = data["class_UID"].get_as<std::string>();
	auto entity_factory_id_opt = CoreEntity::EntityFactoryRegistry::get_instance().get_factory_id(entity_class_UID);
	if (entity_factory_id_opt.has_value())
		this->factory_ID = entity_factory_id_opt.value();
	else
		ResourceLoading::throw_reference_error(std::string(this->uid), entity_class_UID);
}

void CoreEntity::EntityInfo::fill_dependent_data(const DataNode& data) {

}