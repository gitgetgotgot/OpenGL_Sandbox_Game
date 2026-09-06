#pragma once
#include <Objects/ObjectFactory.h>
#include <Objects/ObjectTypes/ArmorInfo.h>

namespace EngineAPI {
	using OBJECT_TYPE = CoreObject::ObjectType;
	template<typename T, OBJECT_TYPE MAIN_TYPE>
	using BASE_OBJECT_INFO_FACTORY = CoreObject::ObjectInfoFactory<T, MAIN_TYPE>;
	using OBJECT_FACTORY_REGISTRY = CoreObject::ObjectFactoryRegistry;
}