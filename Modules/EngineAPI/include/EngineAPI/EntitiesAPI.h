#pragma once
#include <Entities/EntityFactory.h>
#include <Entities/EntityInfoFactory.h>

namespace EngineAPI {
	using BASE_ENTITY_INFO = CoreEntity::EntityInfo;
	using ENTITY_TYPE = CoreEntity::EntityMainType;
	using BASE_ENTITY_CLASS = CoreEntity::EntityBase;
	template<typename T, ENTITY_TYPE MAIN_TYPE>
	using BASE_ENTITY_INFO_FACTORY = CoreEntity::EntityInfoFactory<T, MAIN_TYPE>;
	template<typename T>
	using BASE_ENTITY_CLASS_FACTORY = CoreEntity::EntityFactory<T>;
	using ENTITY_CLASS_FACTORY_REGISTRY = CoreEntity::EntityFactoryRegistry;
	using ENTITY_INFO_FACTORY_REGISTRY = CoreEntity::EntityInfoFactoryRegistry;
}