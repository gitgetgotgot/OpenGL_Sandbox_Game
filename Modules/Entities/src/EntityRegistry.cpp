#include "Entities/EntityRegistry.h"
#include "Entities/EntityInfoFactory.h"
#include "Entities/EntityFactory.h"

#include "Entities/EntityTypes/MobInfo.h"

#include "Entities/EntityClasses/Slime.h"
#include "Entities/EntityClasses/Zombie.h"
#include "Entities/EntityClasses/FlyingEye.h"

void CoreEntity::register_core_entity_info_factories() {
	EntityInfoFactoryRegistry::get_instance().register_factory("Class:Core:Mob", std::make_unique<MobInfoFactory>());
}

void CoreEntity::register_core_entity_factories() {
	EntityFactoryRegistry::get_instance().register_factory("Class:Core:Slime", std::make_unique<SlimeFactory>());
	EntityFactoryRegistry::get_instance().register_factory("Class:Core:Zombie", std::make_unique<ZombieFactory>());
	EntityFactoryRegistry::get_instance().register_factory("Class:Core:FlyingEye", std::make_unique<FlyingEyeFactory>());
}
