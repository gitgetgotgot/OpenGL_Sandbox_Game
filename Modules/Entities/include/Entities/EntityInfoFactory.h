#pragma once
#include "Entities/EntityInfo.h"

namespace CoreEntity {
	class EntityInfoFactoryI {
	public:
		~EntityInfoFactoryI() {}
		virtual void add_entity_info(const DataNode& data) = 0;
	protected:
		EntityInfo* register_entity_info(std::unique_ptr<EntityInfo> new_entity_info, const std::string& UID);
	};

	template<typename T, EntityMainType MAIN_TYPE>
	class EntityInfoFactory : public EntityInfoFactoryI {
	public:
		static_assert(std::is_base_of_v<EntityInfo, T>, "T must inherit from EntityInfo");
		void add_entity_info(const DataNode& data) override {
			T* info = create_entity_info(data);
			info->fill_data(data);
		}
	protected:
		T* create_entity_info(const DataNode& data) {
			std::string UID = data["UID"].get_as<std::string>();
			std::unique_ptr<EntityInfo> new_info = std::make_unique<T>(MAIN_TYPE);
			return static_cast<T*>(register_entity_info(std::move(new_info), UID));
		}
	};

	class EntityInfoFactoryRegistry {
	public:
		static EntityInfoFactoryRegistry& get_instance() {
			static EntityInfoFactoryRegistry registry;
			return registry;
		}
		void ClearData();
		void register_factory(std::string class_name, std::unique_ptr<EntityInfoFactoryI> factory);
		EntityInfoFactoryI* get_factory(uint32_t factory_id);
		std::optional<uint32_t> get_factory_id(std::string entity_class);
	private:
		std::unordered_map<std::string, uint32_t> class_UID_to_factory_ID;
		std::vector<std::unique_ptr<EntityInfoFactoryI>> factories;
		EntityInfoFactoryRegistry() {}
		~EntityInfoFactoryRegistry() {}
	};
}