#pragma once
#include "Objects/ObjectFactory.h"

namespace Engine::Object {
	class ItemInfo : public ObjectInfo {
	public:
		ItemInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ItemInfoFactory : public ObjectInfoFactory<ItemInfo, ObjectType::isItem> {};
}