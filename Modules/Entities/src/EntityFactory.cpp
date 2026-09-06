#include "Entities/EntityFactory.h"

void CoreEntity::EntityFactoryRegistry::ClearData() {
	class_UID_to_factory_ID = {};
	std::vector<std::unique_ptr<EntityFactoryI>>().swap(factories);
}

void CoreEntity::EntityFactoryRegistry::register_factory(std::string class_name, std::unique_ptr<EntityFactoryI> factory) {
	class_UID_to_factory_ID.emplace(class_name, factories.size());
	factories.emplace_back(std::move(factory));
}

CoreEntity::EntityFactoryI* CoreEntity::EntityFactoryRegistry::get_factory(uint16_t factory_id) {
	return factories[factory_id].get();
}

std::optional<uint16_t> CoreEntity::EntityFactoryRegistry::get_factory_id(std::string entity_class) {
	auto it = class_UID_to_factory_ID.find(entity_class);
	if (it == class_UID_to_factory_ID.end()) return std::nullopt;
	else return it->second;
}