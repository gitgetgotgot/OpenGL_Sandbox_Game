#include <Entities/EntityRegistry.h>
#include <Entities/EntityInfoFactory.h>
#include <Entities/EntityFactory.h>

#include <Entities/EntityTypes/MobInfo.h>

#include <Entities/EntityClasses/Slime.h>
#include <Entities/EntityClasses/Zombie.h>
#include <Entities/EntityClasses/FlyingEye.h>

void CoreEntity::register_core_entity_info_factories() {
	EntityInfoFactoryRegistry* registry = EntityInfoFactoryRegistry::get_instance();

	registry->register_factory("Class:Core:Mob", std::make_unique<MobInfoFactory>());
}

void CoreEntity::register_core_entity_factories() {
	EntityFactoryRegistry* registry = EntityFactoryRegistry::get_instance();
	
	registry->register_factory("Class:Core:Slime", std::make_unique<SlimeFactory>());
	registry->register_factory("Class:Core:Zombie", std::make_unique<ZombieFactory>());
	registry->register_factory("Class:Core:FlyingEye", std::make_unique<FlyingEyeFactory>());
}
