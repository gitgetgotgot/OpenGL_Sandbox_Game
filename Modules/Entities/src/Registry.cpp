#include <Entities/Registry.h>
#include <Entities/GameEntities.h>
#include <Entities/Enemies/Slime.h>
#include <Entities/Enemies/Zombie.h>
#include <Entities/Enemies/FlyingEye.h>

void register_core_entity_factories() {
	namespace GE = GameEntity;
	GE::EntityFactoryRegistry* registry = GE::EntityFactoryRegistry::get_instance();
	
	registry->register_factory("Class:Core:Slime", std::make_unique<GE::SlimeFactory>());
	registry->register_factory("Class:Core:Zombie", std::make_unique<GE::ZombieFactory>());
	registry->register_factory("Class:Core:FlyingEye", std::make_unique<GE::FlyingEyeFactory>());
}
