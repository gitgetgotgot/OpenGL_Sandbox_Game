#pragma once
#include <Objects/GameObjects.h>

class ArmorInfo : public ObjectInfo {
public:
	ArmorInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}
};

class ArmorInfoFactory : public ObjectInfoFactory<ArmorInfo, ObjectType::isArmor> {};