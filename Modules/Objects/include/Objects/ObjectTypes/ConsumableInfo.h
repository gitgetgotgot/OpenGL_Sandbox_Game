#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class ConsumableInfo : public ObjectInfo {
	public:
		ConsumableInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ConsumableInfoFactory : public ObjectInfoFactory<ConsumableInfo, ObjectType::isConsumable> {};
}