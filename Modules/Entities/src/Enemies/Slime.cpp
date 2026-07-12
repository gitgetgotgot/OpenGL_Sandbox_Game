#include <Entities/Enemies/Slime.h>

void Slime::update_entity(float deltaTime) {
	if (physics.has_bottom_collision || physics.has_bottom_collision_only_with_objects) {
		time_standing += deltaTime;
		physics.Xinc = 0.f;
	}
	update_hit_cd(deltaTime);
	sprite_time += deltaTime;
	//change sprite every half second
	if (sprite_time >= 0.25f) {
		sprite_time = 0.f;
		current_sprite++;
		if (current_sprite > 1)
			current_sprite = 0;
	}
	if (physics.time_in_free_falling > 0.f)
		current_sprite = 1;
	//make jump after standing for 5 seconds
	if (time_standing >= 5.f) {
		time_standing = 0.f;
		physics.should_jump = true;
		if (GameContext::PLAYER_LAST_POS.x - hitbox.center.x > 0.f)
			physics.Xinc = physics.walk_V * deltaTime;
		else
			physics.Xinc = -physics.walk_V * deltaTime;
	}
}

