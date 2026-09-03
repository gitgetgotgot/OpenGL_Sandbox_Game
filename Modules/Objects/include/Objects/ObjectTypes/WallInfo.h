#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class WallInfo : public ObjectInfo {
	public:
		WallInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		float toughness = 1.0f;
	};

	class WallInfoFactory : public ObjectInfoFactory<WallInfo, ObjectType::isWall> {};
}