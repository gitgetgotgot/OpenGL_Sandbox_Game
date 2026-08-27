#pragma once
#include <Objects/GameObjects.h>

class ItemInfo : public ObjectInfo {
public:
	ItemInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}
};

class ItemInfoFactory : public ObjectInfoFactory<ItemInfo, ObjectType::isItem> {};