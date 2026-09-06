#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class BlockInfo : public ObjectInfo {
	public:
		BlockInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		float toughness = 1;
		bool collision = false;
		bool platform_collision = false; //object behaves like a platform with only one direction collision
	};

	class BlockInfoFactory : public ObjectInfoFactory<BlockInfo, ObjectType::isBlock> {};
}