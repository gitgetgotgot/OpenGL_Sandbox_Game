#pragma once
#include "Entities/EntityInfo.h"
#include <deque>

namespace CoreEntity {
	class EntityInfoManager {
	public:
		static EntityInfoManager& get_instance() {
			static EntityInfoManager mgr;
			return mgr;
		}
		EntityInfo* register_entity_info(std::unique_ptr<EntityInfo> new_entity_info, const std::string& UID);
		void ClearData();
		EntityInfo* get_entity_info(uint16_t ID);
		std::optional<uint16_t> get_entity_id(std::string_view UID);
	private:
		std::deque<std::string> UID_storage;
		std::unordered_map<std::string_view, uint16_t> UID_to_ID;
		std::vector<std::unique_ptr<EntityInfo>> entities;
	};
}