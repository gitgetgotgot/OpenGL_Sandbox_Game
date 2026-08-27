#pragma once
#include <Objects/GameObjects.h>

enum MultiBlockType : uint8_t {
	isWorkbench = 0, isFurnace = 1, isChest = 2, isDoor = 3, isTable = 4, isChair = 5, isAnvil = 6, isLamp = 7, isChandelier = 8,
	isLyfeCrystal = 9
};

class MultiBlockInfo : public ObjectInfo {
public:
	MultiBlockInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}

	MultiBlockType multi_block_type = MultiBlockType::isWorkbench;
	float tiles_width = 0, tiles_height = 0;
	float toughness = 1;
	bool collision = false;
	bool platform_collision = false; //object behaves like a platform with only one direction collision
};

class MultiBlockInfoFactory : public ObjectInfoFactory<MultiBlockInfo, ObjectType::isMultiBlock> {
public:
	void add_object(DataNode& data) override {
		ObjectInfo* info = create_object_info(data);
		try {
			uint8_t sub_type_id = data["sub_type"].get_as<uint8_t>();
			float tiles_width = data["width"].get_as<float>();
			float tiles_height = data["height"].get_as<float>();
			float toughness = data["toughness"].get_as<float>();
			bool collision = false, platform_collision = false;

			if (data.has_child("collision"))
				collision = data["collision"].get_as<bool>();
			if (data.has_child("platform_collision"))
				platform_collision = data["platform_collision"].get_as<bool>();

			static_cast<MultiBlockInfo*>(info)->multi_block_type = MultiBlockType(sub_type_id);
			static_cast<MultiBlockInfo*>(info)->tiles_width = tiles_width;
			static_cast<MultiBlockInfo*>(info)->tiles_height = tiles_height;
			static_cast<MultiBlockInfo*>(info)->toughness = toughness;
			static_cast<MultiBlockInfo*>(info)->collision = collision;
			static_cast<MultiBlockInfo*>(info)->platform_collision = platform_collision;
		}
		catch (const std::exception& e) {
			GameObjects::throw_resource_error(info->uid, e.what());
		}
	}
};