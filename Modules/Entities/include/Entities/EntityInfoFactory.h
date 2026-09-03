#pragma once
#include <Entities/GameEntities.h>

namespace CoreEntity {
	class EntityInfoFactoryI {
	public:
		~EntityInfoFactoryI() {}
		//virtual main method to add info
		virtual void add_entity_info(const DataNode& data) = 0;
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
			return static_cast<T*>(EntityInfoManager::get_instance()->add_entity_info<T, MAIN_TYPE>(data));
		}
	};

	class EntityInfoFactoryRegistry {
	public:
		static EntityInfoFactoryRegistry* get_instance() {
			static EntityInfoFactoryRegistry registry;
			return &registry;
		}
		void ClearData() {
			class_UID_to_factory_ID = {};
			std::vector<std::unique_ptr<EntityInfoFactoryI>>().swap(factories);
		}
		void register_factory(std::string class_name, std::unique_ptr<EntityInfoFactoryI> factory) {
			class_UID_to_factory_ID.emplace(class_name, factories.size());
			factories.emplace_back(std::move(factory));
		}
		EntityInfoFactoryI* get_factory(uint32_t factory_id) {
			return factories[factory_id].get();
		}
		std::optional<uint32_t> get_factory_id(std::string entity_class) {
			auto it = class_UID_to_factory_ID.find(entity_class);
			if (it == class_UID_to_factory_ID.end()) return std::nullopt;
			else return it->second;
		}
	private:
		std::unordered_map<std::string, uint32_t> class_UID_to_factory_ID;
		std::vector<std::unique_ptr<EntityInfoFactoryI>> factories;
		EntityInfoFactoryRegistry() {}
		~EntityInfoFactoryRegistry() {}
	};
}