#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	enum ConsumableType : uint8_t {
		isFood = 0, isPotion = 1
	};

	class ConsumableInfo : public ObjectInfo {
	public:
		ConsumableInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ConsumableInfoFactory : public ObjectInfoFactory<ConsumableInfo, ObjectType::isConsumable> {};
}