#pragma once
#include <Entities/EntityInfoFactory.h>
#include <Resources/ResourceData.h>

namespace Engine::Entity {
	class MobInfo : public EntityInfo {
	public:
		MobInfo(EntityMainType type) : EntityInfo(type) {}
		void fill_data(const nlohmann::json& data) override {
			EntityInfo::fill_data(data);

		}
	public:
		float HP = 0.0f, DMG = 0.0f, DEF = 0.0f;
		Engine::Util::DynamicArray<Engine::Resource::EntityDropEntry> drops;
		Engine::Util::DynamicArray<Engine::Resource::EffectEntry> effects;
		float speed_x = 0.0f, speed_y = 0.0f;
		uint32_t animator_id = 0;
		glm::vec2 hitboxSize{ 1.0f };
	};

	class MobInfoFactory : public EntityInfoFactory<MobInfo, EntityMainType::isMob> {};
}
