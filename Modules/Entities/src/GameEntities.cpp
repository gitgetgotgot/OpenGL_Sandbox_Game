#include <Entities/GameEntities.h>

CoreEntity::EntityInfo::EntityInfo(CoreEntity::EntityMainType type, std::string_view UID) :
	main_type{ type }, uid{ UID } {
	size_t pos = uid.rfind(':');
	name = uid.substr(pos + 1);
}

void CoreEntity::EntityInfo::fill_data(const DataNode& data) {

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
void CoreEntity::EntityInfoManager::fill_base_data(const DataNode& data, CoreEntity::EntityInfo* info) {

}