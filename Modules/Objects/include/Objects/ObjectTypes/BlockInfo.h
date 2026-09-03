#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	enum BlockType : uint8_t {
		isSolidBlock = 0, isWood = 1, isTreeTop = 2, isPlatform = 3, isTorch = 4, isLiquid = 5, isGrass = 6, isPlant = 7, isGlass = 8,
		isBottle = 9
	};

	class BlockInfo : public ObjectInfo {
	public:
		BlockInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		BlockType block_type = BlockType::isSolidBlock;
		float toughness = 1;
		bool collision = false;
		bool platform_collision = false; //object behaves like a platform with only one direction collision
	};

	class BlockInfoFactory : public ObjectInfoFactory<BlockInfo, ObjectType::isBlock> {};
}