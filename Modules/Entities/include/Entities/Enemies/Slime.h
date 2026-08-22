#pragma once
#include <Entities/GameEntities.h>

namespace GameEntity {
	class Slime : public Mob {
		uint32_t anim_idle = 0, anim_jump = 0;
	public:
		Slime(uint32_t entity_id, glm::vec2 spawn_center) : Mob(entity_id, spawn_center) {}
		void on_create() override;
		void on_destroy() override;
		void update() override;
	private:
		void change_anim();
		enum SLIME_ANIM_STATE : uint8_t {
			SLIME_IDLE, SLIME_JUMP
		} current_state = SLIME_IDLE, prev_state = SLIME_JUMP;
		float time_standing = 0.f; //time when slime is not moving
	};

	class SlimeFactory : public EntityFactory {
	public:
		std::unique_ptr<EntityBase> spawn_entity(
			uint32_t entity_global_ID,
			float world_x, float world_y
		) override {
			return std::make_unique<Slime>(entity_global_ID, glm::vec2(world_x, world_y));
		}
	};
}
