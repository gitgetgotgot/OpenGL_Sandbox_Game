#pragma once
#include <Entities/EntityFactory.h>
#include <Entities/EntityInfoFactory.h>

namespace EngineAPI {
	using BASE_ENTITY_INFO = Engine::Entity::EntityInfo;
	using ENTITY_TYPE = Engine::Entity::EntityMainType;
	using BASE_ENTITY_CLASS = Engine::Entity::EntityBase;
	template<typename T, ENTITY_TYPE MAIN_TYPE>
	using BASE_ENTITY_INFO_FACTORY = Engine::Entity::EntityInfoFactory<T, MAIN_TYPE>;
	template<typename T>
	using BASE_ENTITY_CLASS_FACTORY = Engine::Entity::EntityFactory<T>;
	using ENTITY_CLASS_FACTORY_REGISTRY = Engine::Entity::EntityFactoryRegistry;
	using ENTITY_INFO_FACTORY_REGISTRY = Engine::Entity::EntityInfoFactoryRegistry;
}