#pragma once
#include "Objects/ObjectFactory.h"

namespace Engine::Object {
	class AmmoInfo : public ObjectInfo {
	public:
		AmmoInfo(ObjectType type) : ObjectInfo(type) {}
		void fill_data(const DataNode& data) override;
		uint32_t entity_id;
		uint32_t damage;
	};

	class AmmoInfoFactory : public ObjectInfoFactory<AmmoInfo, ObjectType::isAmmo> {};
}