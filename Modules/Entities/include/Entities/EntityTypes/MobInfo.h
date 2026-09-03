#pragma once
#include <Entities/EntityInfoFactory.h>

namespace CoreEntity {
	class MobInfo : public EntityInfo {
	public:
		MobInfo(EntityMainType type, std::string_view UID) : EntityInfo(type, UID) {}
		void fill_data(const DataNode& data) override {
			EntityInfo::fill_data(data);

		}
	public:
		float HP = 0.0f, DMG = 0.0f, DEF = 0.0f;
		DynamicArray<DropInfo> drops;
		float speed_x = 0.0f, speed_y = 0.0f;
		uint32_t animator_id = 0;
		MovementType move_type = MovementType::isWalking;
		glm::vec2 hitboxSize{ 1.0f };
	};

	class MobInfoFactory : public EntityInfoFactory<MobInfo, EntityMainType::isMob> {};
}
