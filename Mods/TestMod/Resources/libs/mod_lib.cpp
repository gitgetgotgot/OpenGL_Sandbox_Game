#include <EngineAPI/Macros.h>
#include <EngineAPI/ObjectFactoryAPI.h>
#include <EngineAPI/BaseArmor.h>

class ModArmorInfo : public EngineAPI::BASE_ARMOR_INFO {};

class ModArmorInfoFactory : public EngineAPI::BASE_OBJECT_INFO_FACTORY<ModArmorInfo, EngineAPI::OBJECT_TYPE::isArmor> {};

EXPORT_FUNCTION void REGISTER_OBJECT_FACTORIES(EngineAPI::OBJECT_FACTORY_REGISTRY* registry) {
	registry->register_factory("Class:TestMod:ModArmor", std::make_unique<ModArmorInfoFactory>());
}