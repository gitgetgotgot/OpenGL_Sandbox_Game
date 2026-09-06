#include "Entities/EntityInfoManager.h"

CoreEntity::EntityInfo* CoreEntity::EntityInfoManager::register_entity_info(
	std::unique_ptr<EntityInfo> new_entity_info, const std::string& UID
) {
	std::string_view uid_view = UID_storage.emplace_back(UID);
	UID_to_ID.emplace(uid_view, entities.size());
	new_entity_info->_set_UID(uid_view);
	return entities.emplace_back(std::move(new_entity_info)).get();
}

void CoreEntity::EntityInfoManager::ClearData() {
	UID_storage = {};
	UID_to_ID = {};
	std::vector<std::unique_ptr<EntityInfo>>().swap(entities);
}
CoreEntity::EntityInfo* CoreEntity::EntityInfoManager::get_entity_info(uint16_t ID) {
	return entities[ID].get();
}
std::optional<uint16_t> CoreEntity::EntityInfoManager::get_entity_id(std::string_view UID) {
	auto it = UID_to_ID.find(UID);
	if (it == UID_to_ID.end()) return std::nullopt;
	else return it->second;
}
