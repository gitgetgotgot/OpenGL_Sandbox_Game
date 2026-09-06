#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class MultiBlockInfo : public ObjectInfo {
	public:
		MultiBlockInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		float tiles_width = 0, tiles_height = 0;
		float toughness = 1;
		bool collision = false;
		bool platform_collision = false; //object behaves like a platform with only one direction collision
	};

	class MultiBlockInfoFactory : public ObjectInfoFactory<MultiBlockInfo, ObjectType::isMultiBlock> {};
}