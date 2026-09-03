#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	enum MultiBlockType : uint8_t {
		isWorkbench = 0, isFurnace = 1, isChest = 2, isDoor = 3, isTable = 4, isChair = 5, isAnvil = 6, isLamp = 7, isChandelier = 8,
		isLyfeCrystal = 9
	};

	class MultiBlockInfo : public ObjectInfo {
	public:
		MultiBlockInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		MultiBlockType multi_block_type = MultiBlockType::isWorkbench;
		float tiles_width = 0, tiles_height = 0;
		float toughness = 1;
		bool collision = false;
		bool platform_collision = false; //object behaves like a platform with only one direction collision
	};

	class MultiBlockInfoFactory : public ObjectInfoFactory<MultiBlockInfo, ObjectType::isMultiBlock> {};
}