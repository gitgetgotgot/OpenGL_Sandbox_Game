#pragma once
#include <Objects/GameObjects.h>

class WallInfo : public ObjectInfo {
public:
	WallInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}
	float toughness = 1.0f;
};

class WallInfoFactory : public ObjectInfoFactory<WallInfo, ObjectType::isWall> {
public:
	void add_object(DataNode& data) override {
		ObjectInfo* info = create_object_info(data);
		try {
			float toughness = data["toughness"].get_as<float>();
			static_cast<WallInfo*>(info)->toughness = toughness;
		}
		catch (const std::exception& e) {
			GameObjects::throw_resource_error(info->uid, e.what());
		}
	}
};