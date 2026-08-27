#pragma once
#include <Objects/GameObjects.h>

enum BlockType : uint8_t {
	isSolidBlock = 0, isWood = 1, isTreeTop = 2, isPlatform = 3, isTorch = 4, isLiquid = 5, isGrass = 6, isPlant = 7, isGlass = 8,
	isBottle = 9
};

class BlockInfo : public ObjectInfo {
public:
	BlockInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}

	BlockType block_type = BlockType::isSolidBlock;
	float toughness = 1;
	bool collision = false;
	bool platform_collision = false; //object behaves like a platform with only one direction collision
};

class BlockInfoFactory : public ObjectInfoFactory<BlockInfo, ObjectType::isBlock> {
public:
	void add_object(DataNode& data) override {
		ObjectInfo* info = create_object_info(data);
		try {
			uint8_t sub_type_id = data["sub_type"].get_as<uint8_t>();
			float toughness = data["toughness"].get_as<float>();
			bool collision = false, platform_collision = false;

			if (data.has_child("collision"))
				collision = data["collision"].get_as<bool>();
			if (data.has_child("platform_collision"))
				platform_collision = data["platform_collision"].get_as<bool>();

			static_cast<BlockInfo*>(info)->block_type = BlockType(sub_type_id);
			static_cast<BlockInfo*>(info)->toughness = toughness;
			static_cast<BlockInfo*>(info)->collision = collision;
			static_cast<BlockInfo*>(info)->platform_collision = platform_collision;
		}
		catch (const std::exception& e) {
			GameObjects::throw_resource_error(info->uid, e.what());
		}
	}
};