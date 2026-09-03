#pragma once
#include "Objects/ObjectFactory.h"

namespace CoreObject {
	class ArtifactInfo : public ObjectInfo {
	public:
		ArtifactInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ArtifactInfoFactory : public ObjectInfoFactory<ArtifactInfo, ObjectType::isArtifact> {};
}