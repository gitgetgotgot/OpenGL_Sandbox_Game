#pragma once
#include "Objects/ObjectInfo.h"
#include <deque>

namespace CoreObject {
	class ObjectManager {
	public:
		static ObjectManager* get_instance();
		template<typename ClassInfoT, ObjectType MAIN_TYPE>
		void create_object_info(std::string UID) {
			static_assert(std::is_base_of_v<ObjectInfo, ClassInfoT>, "ClassInfoT must inherit from ObjectInfo");
			std::string_view uid_view = UID_storage.emplace_back(UID);
			UID_to_ID.emplace(uid_view, objects.size());
			objects.emplace_back(std::make_unique<ClassInfoT>(MAIN_TYPE))->set_UID(uid_view);
		}
		ObjectInfo* register_object_info(std::unique_ptr<ObjectInfo> new_object_info, const std::string& UID);
		void ClearData();
		ObjectInfo* get_object_info(uint16_t ID);
		std::optional<uint16_t> get_object_id(std::string_view UID);
	private:
		std::deque<std::string> UID_storage;
		std::unordered_map<std::string_view, uint16_t> UID_to_ID;
		std::vector<std::unique_ptr<ObjectInfo>> objects;
	};
}