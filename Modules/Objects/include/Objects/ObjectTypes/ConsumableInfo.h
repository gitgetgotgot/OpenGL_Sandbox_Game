#pragma once
#include "Objects/ObjectFactory.h"

namespace Engine::Object {
	class ConsumableInfo : public ObjectInfo {
	public:
		ConsumableInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ConsumableInfoFactory : public ObjectInfoFactory<ConsumableInfo, ObjectType::isConsumable> {};
}