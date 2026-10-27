#include "Entities/EntityInfoFactory.h"
#include "Entities/EntityInfoManager.h"

Engine::Entity::EntityInfo* Engine::Entity::EntityInfoFactoryI::register_entity_info(std::unique_ptr<EntityInfo> new_entity_info, const std::string& UID) {
	return EntityInfoManager::Instance().register_entity_info(std::move(new_entity_info), UID);
}

void Engine::Entity::EntityInfoFactoryRegistry::ClearData() {
	class_UID_to_factory_ID = {};
	std::vector<std::unique_ptr<EntityInfoFactoryI>>().swap(factories);
}

void Engine::Entity::EntityInfoFactoryRegistry::register_factory(std::string class_name, std::unique_ptr<EntityInfoFactoryI> factory) {
	class_UID_to_factory_ID.emplace(class_name, factories.size());
	factories.emplace_back(std::move(factory));
}

Engine::Entity::EntityInfoFactoryI* Engine::Entity::EntityInfoFactoryRegistry::get_factory(uint32_t factory_id) {
	return factories[factory_id].get();
}

std::optional<uint32_t> Engine::Entity::EntityInfoFactoryRegistry::get_factory_id(std::string entity_class) {
	auto it = class_UID_to_factory_ID.find(entity_class);
	if (it == class_UID_to_factory_ID.end()) return std::nullopt;
	else return it->second;
}