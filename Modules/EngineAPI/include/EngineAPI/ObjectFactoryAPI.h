#pragma once
#include <Objects/ObjectFactory.h>
#include <Objects/ObjectTypes/ArmorInfo.h>

namespace EngineAPI {
	using OBJECT_TYPE = Engine::Object::ObjectType;
	template<typename T, OBJECT_TYPE MAIN_TYPE>
	using BASE_OBJECT_INFO_FACTORY = Engine::Object::ObjectInfoFactory<T, MAIN_TYPE>;
	using OBJECT_FACTORY_REGISTRY = Engine::Object::ObjectFactoryRegistry;
}