#pragma once
#include <Objects/GameObjects.h>

enum ConsumableType : uint8_t {
	isFood = 0, isPotion = 1
};

class ConsumableInfo : public ObjectInfo {
public:
	ConsumableInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}
};

class ConsumableInfoFactory : public ObjectInfoFactory<ConsumableInfo, ObjectType::isConsumable> {};