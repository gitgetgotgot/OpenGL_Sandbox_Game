#pragma once
#include <Entities/GameEntities.h>

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
	} current_state = SLIME_IDLE, prev_state = SLIME_IDLE;
	float time_standing = 0.f; //time when slime is not moving
};