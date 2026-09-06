#pragma once
#include "Entities/EntityBase.h"
#include <memory>

namespace CoreEntity {
	class EntityFactoryI {
	public:
		~EntityFactoryI() {}
		virtual std::unique_ptr<EntityBase> spawn(uint32_t entity_global_ID, float world_x, float world_y) = 0;
	};

	template<typename T>
	class EntityFactory : public EntityFactoryI {
	public:
		static_assert(std::is_base_of_v<EntityBase, T>, "T must inherit from EntityBase");
		std::unique_ptr<EntityBase> spawn(uint32_t entity_global_ID, float world_x, float world_y) override {
			return spawn_entity(entity_global_ID, world_x, world_y);
		}
		virtual std::unique_ptr<EntityBase> spawn_entity(uint32_t entity_global_ID, float world_x, float world_y) {
			return std::make_unique<T>(entity_global_ID, glm::vec2(world_x, world_y));
		}
	};

	class EntityFactoryRegistry {
	public:
		static EntityFactoryRegistry& get_instance() {
			static EntityFactoryRegistry registry;
			return registry;
		}
		void ClearData();
		void register_factory(std::string class_name, std::unique_ptr<EntityFactoryI> factory);
		EntityFactoryI* get_factory(uint16_t factory_id);
		std::optional<uint16_t> get_factory_id(std::string entity_class);
	private:
		std::unordered_map<std::string, uint16_t> class_UID_to_factory_ID;
		std::vector<std::unique_ptr<EntityFactoryI>> factories;
		EntityFactoryRegistry() {}
		~EntityFactoryRegistry() {}
	};
}