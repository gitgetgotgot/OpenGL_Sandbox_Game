#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class ItemInfo : public ObjectInfo {
	public:
		ItemInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ItemInfoFactory : public ObjectInfoFactory<ItemInfo, ObjectType::isItem> {};
}