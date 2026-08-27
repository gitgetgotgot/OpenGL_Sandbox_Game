#pragma once
#include <Objects/GameObjects.h>

class ArtifactInfo : public ObjectInfo {
public:
	ArtifactInfo(ObjectType type, std::string_view uid) : ObjectInfo(type, uid) {}
};

class ArtifactInfoFactory : public ObjectInfoFactory<ArtifactInfo, ObjectType::isArtifact> {};