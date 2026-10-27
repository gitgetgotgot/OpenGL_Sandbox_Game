#include "Objects/ObjectRegistry.h"
#include "Objects/ObjectFactory.h"
#include "Objects/ObjectTypes/WallInfo.h"
#include "Objects/ObjectTypes/BlockInfo.h"
#include "Objects/ObjectTypes/MultiBlockInfo.h"
#include "Objects/ObjectTypes/ItemInfo.h"
#include "Objects/ObjectTypes/WeaponInfo.h"
#include "Objects/ObjectTypes/AmmoInfo.h"
#include "Objects/ObjectTypes/ConsumableInfo.h"
#include "Objects/ObjectTypes/ArmorInfo.h"
#include "Objects/ObjectTypes/ArtifactInfo.h"

void Engine::Object::register_core_object_factories() {
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Wall",		std::make_unique<WallInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Block",		std::make_unique<BlockInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:MultiBlock", std::make_unique<MultiBlockInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Item",		std::make_unique<ItemInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Weapon",		std::make_unique<WeaponInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Ammo",		std::make_unique<AmmoInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Consumable", std::make_unique<ConsumableInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Armor",		std::make_unique<ArmorInfoFactory>());
	ObjectFactoryRegistry::Instance().register_factory("Class:Core:Artifact",	std::make_unique<ArtifactInfoFactory>());
}
