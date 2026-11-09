#include "Entities/EntityInfoManager.h"
#include <Utility/Logger.h>
#include <Entities/EntityInfoFactory.h>

Engine::Entity::EntityInfo* Engine::Entity::EntityInfoManager::register_entity_info(
	std::unique_ptr<EntityInfo> new_entity_info, const std::string& UID
) {
	std::string_view uid_view = UID_storage.emplace_back(UID);
	UID_to_ID.emplace(uid_view, entities.size());
	new_entity_info->_set_UID(uid_view);
	return entities.emplace_back(std::move(new_entity_info)).get();
}

void Engine::Entity::EntityInfoManager::ClearData() {
	UID_storage = {};
	UID_to_ID = {};
	std::vector<std::unique_ptr<EntityInfo>>().swap(entities);
}

bool Engine::Entity::EntityInfoManager::load_entities_from_data(
	const std::filesystem::path& data_file_path,
	const Engine::Util::DataParser& parser
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		std::string info_class_uid;
		if (DP::get_json_value<std::string, DP::JsonValue::V_STRING>(info_class_uid, j, "info_class_UID") == false) {
			Engine::Util::Logger::Log("Invalid syntax for Entity in <" + data_file_path.string() + ">!");
			return false;
		}
		auto factory_id = Engine::Entity::EntityInfoFactoryRegistry::Instance().get_factory_id(info_class_uid);
		if (!factory_id) {
			Engine::Util::Logger::Log("Referenced Class <" + info_class_uid + "> not found in >" + data_file_path.string() + ">!");
			return false;
		}
		Engine::Entity::EntityInfoFactoryI* factory = Engine::Entity::EntityInfoFactoryRegistry::Instance().get_factory(factory_id.value());
		if (factory->add_entity_info(j) == false) {
			return false;
		}
	}
	return true;
}

Engine::Entity::EntityInfo* Engine::Entity::EntityInfoManager::get_entity_info(uint16_t ID) {
	return entities[ID].get();
}

std::optional<uint16_t> Engine::Entity::EntityInfoManager::get_entity_id(const std::string& UID) {
	auto it = UID_to_ID.find(UID);
	if (it == UID_to_ID.end()) return std::nullopt;
	else return it->second;
}
