#include <Entities/Enemies/Slime.h>
#include <Utility/TimeManager.h>

void GameEntity::Slime::on_create() {
	anim_idle = anim_controller.get_clip_id(0);
	anim_jump = anim_controller.get_clip_id(1);
	anim_controller.play(anim_idle);
}

void GameEntity::Slime::on_destroy() {

}

void GameEntity::Slime::update() {
	anim_controller.update();

	if (physics.collision & CollisionType::BOTTOM || physics.platform_collision) {
		time_standing += TimeManager::deltaTime;
		physics.linear_velocity.x = 0.0f;
		current_state = SLIME_ANIM_STATE::SLIME_IDLE;
	}
	else if (physics.linear_velocity.x == 0.0f) {
		if (GameContext::PLAYER_LAST_POS.x - hitbox.center.x > 0.f)
			physics.linear_velocity.x = 4.0f;
		else
			physics.linear_velocity.x = -4.0f;
	}

	if (physics.time_falling > 0.0f)
		current_state = SLIME_ANIM_STATE::SLIME_JUMP;

	//make jump after standing for 5 seconds
	if (time_standing >= 5.0f) {
		time_standing = 0.0f;
		physics.linear_velocity.y = 8.0f;
	}

	if (current_state != prev_state) {
		change_anim();
		prev_state = current_state;
	}
	
	current_sprite = anim_controller.current_sprite;
}

void GameEntity::Slime::change_anim() {
	switch (current_state) {
	case SLIME_ANIM_STATE::SLIME_IDLE:
		anim_controller.play(anim_idle);
		break;
	case SLIME_ANIM_STATE::SLIME_JUMP:
		anim_controller.play(anim_jump);
		break;
	}
}