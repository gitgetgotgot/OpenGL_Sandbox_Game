#pragma once
#include "Objects/ObjectFactory.h"

namespace Engine::Object {
	class ArtifactInfo : public ObjectInfo {
	public:
		ArtifactInfo(ObjectType type) : ObjectInfo(type) {}
	};

	class ArtifactInfoFactory : public ObjectInfoFactory<ArtifactInfo, ObjectType::isArtifact> {};
}