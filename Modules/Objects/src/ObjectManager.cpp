#include "Objects/ObjectManager.h"

CoreObject::ObjectManager* CoreObject::ObjectManager::get_instance() {
	static CoreObject::ObjectManager mgr;
	return &mgr;
}

CoreObject::ObjectInfo* CoreObject::ObjectManager::register_object_info(
	std::unique_ptr<CoreObject::ObjectInfo> new_object_info, const std::string& UID
) {
	std::string_view uid_view = UID_storage.emplace_back(UID);
	UID_to_ID.emplace(uid_view, objects.size());
	new_object_info->set_UID(uid_view);
	return objects.emplace_back(std::move(new_object_info)).get();
}

void CoreObject::ObjectManager::ClearData() {
	UID_storage = {};
	UID_to_ID = {};
	std::vector<std::unique_ptr<ObjectInfo>>().swap(objects);
}

CoreObject::ObjectInfo* CoreObject::ObjectManager::get_object_info(uint16_t ID) {
	return objects[ID].get();
}

std::optional<uint16_t> CoreObject::ObjectManager::get_object_id(std::string_view UID) {
	auto it = UID_to_ID.find(UID);
	if (it == UID_to_ID.end()) return std::nullopt;
	else return it->second;
}