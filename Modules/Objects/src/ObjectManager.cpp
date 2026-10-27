#include "Objects/ObjectManager.h"

Engine::Object::ObjectInfo* Engine::Object::ObjectManager::register_object_info(
	std::unique_ptr<Engine::Object::ObjectInfo> new_object_info, const std::string& UID
) {
	std::string_view uid_view = UID_storage.emplace_back(UID);
	UID_to_ID.emplace(uid_view, objects.size());
	new_object_info->_set_UID(uid_view);
	return objects.emplace_back(std::move(new_object_info)).get();
}

void Engine::Object::ObjectManager::setup() {
	//add "Air" info with global_ID = 0
	create_object_info<Engine::Object::ObjectInfo, Engine::Object::ObjectType::None>("Item:Core:Air");
	//add "MultiBlockTile" info with global_ID = 1
	create_object_info<Engine::Object::ObjectInfo, Engine::Object::ObjectType::isMultiBlockTile>("Item:Core:MultiBlockTile");
}

void Engine::Object::ObjectManager::ClearData() {
	UID_storage = {};
	UID_to_ID = {};
	std::vector<std::unique_ptr<ObjectInfo>>().swap(objects);
}

Engine::Object::ObjectInfo* Engine::Object::ObjectManager::get_object_info(uint16_t ID) {
	return objects[ID].get();
}

std::optional<uint16_t> Engine::Object::ObjectManager::get_object_id(std::string_view UID) {
	auto it = UID_to_ID.find(UID);
	if (it == UID_to_ID.end()) return std::nullopt;
	else return it->second;
}

void Engine::Object::ObjectManager::build_ID_to_UID_table(std::vector<std::string_view>& table) {
	size_t size = objects.size();
	table.resize(size);
	for (size_t i = 0; i < size; ++i) {
		table[i] = objects[i]->uid;
	}
}