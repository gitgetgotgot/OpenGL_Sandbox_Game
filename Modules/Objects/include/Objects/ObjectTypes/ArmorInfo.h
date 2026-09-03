#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class ArmorInfo : public ObjectInfo {
	public:
		ArmorInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ArmorInfoFactory : public ObjectInfoFactory<ArmorInfo, ObjectType::isArmor> {};
}