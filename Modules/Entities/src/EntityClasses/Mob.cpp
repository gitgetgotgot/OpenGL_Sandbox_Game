#include "Entities/EntityClasses/Mob.h"
#include "Entities/EntityTypes/MobInfo.h"
#include "Entities/EntityInfoManager.h"

CoreEntity::Mob::Mob(uint32_t entity_id, glm::vec2 spawn_center) : EntityBase(entity_id) {
	CoreEntity::MobInfo* info = static_cast<CoreEntity::MobInfo*>(CoreEntity::EntityInfoManager::get_instance().get_entity_info(entity_id));
	stats.HP = info->HP;
	stats.DEF = info->DEF;
	stats.speed = glm::vec2(info->speed_x, info->speed_y);

	transform.pos = spawn_center;
	hitbox.size = info->hitboxSize;
	hitbox.center = spawn_center; //probably incorrect

	anim_controller.set_animator(info->animator_id);

	physics.current_Y_max_level = hitbox.center.y - hitbox.size.y * 0.5f;
}