#pragma once
#include <Entities/EntityTypes/MobInfo.h>

namespace CoreEntity {
	class Mob : public EntityBase {
	public:
		Mob(uint32_t entity_id, glm::vec2 spawn_center) : EntityBase(entity_id) {
			MobInfo* info = static_cast<MobInfo*>(EntityInfoManager::get_instance()->get_entity_info(entity_id));
			stats.HP = info->HP;
			stats.DEF = info->DEF;
			stats.speed = glm::vec2(info->speed_x, info->speed_y);

			transform.pos = spawn_center;
			hitbox.size = info->hitboxSize;
			hitbox.center = spawn_center; //probably incorrect

			anim_controller.set_animator(info->animator_id);

			physics.current_Y_max_level = hitbox.center.y - hitbox.size.y * 0.5f;
		}

		CoreAnimation::AnimatorController anim_controller;
		MobStats stats;
		MobPhysics physics;
		Collider_2D_AABB hitbox;
	};
}