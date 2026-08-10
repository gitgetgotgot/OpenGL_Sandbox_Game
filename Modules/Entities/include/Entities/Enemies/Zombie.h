#pragma once
#include <Entities/GameEntities.h>

class Zombie : public Mob {
	uint32_t anim_idle = 0, anim_walk = 0, anim_jump = 0;
public:
	Zombie(uint32_t entity_id, glm::vec2 spawn_center) : Mob(entity_id, spawn_center) {}
	void on_create() override;
	void on_destroy() override;
	void update() override;

	void update_entity(float deltaTime) {
		//manage zombie sprites
		/*
		sprite_time += deltaTime;
		sounds_cd_time += deltaTime;
		if (physics.has_bottom_collision || physics.has_bottom_collision_only_with_objects) {
			if (physics.Xinc != 0.f) {
				if (current_sprite == 0)
					current_sprite = 2;
				else if (sprite_time >= 0.2f) {
					sprite_time = 0.f;
					current_sprite++;
					if (current_sprite > 5)
						current_sprite = 2;
				}
			}
			else
				current_sprite = 0;
		}
		else {
			current_sprite = 1;
		}
		//try to jump if current Xinc is 0, meaning that there is no movement
		if (physics.has_side_collision && !physics.has_top_collision && (physics.has_bottom_collision || physics.has_bottom_collision_only_with_objects)) {
			time_standing = 0.f;
			physics.should_jump = true;
		}
		//Xinc
		if (GameContext::PLAYER_LAST_POS.x + sign * 0.75 - hitbox.center.x > 0.f) {
			if (!player_right_side) {
				player_right_side = !player_right_side;
				sign = 1;
			}
			physics.Xinc = physics.walk_V * deltaTime;
			flip_x = false;
		}
		else {
			if (player_right_side) {
				player_right_side = !player_right_side;
				sign = -1;
			}
			physics.Xinc = -physics.walk_V * deltaTime;
			flip_x = true;
		}
		*/
	}
private:
	float sounds_cd_time = 0.f; //cooldown for making sounds
	float time_standing = 0.f; //time when zombie is not moving right or left
	bool player_right_side = false; //goes to the right or left side of player (used to prevent zombie from moving right and left on one place when at the same X position as player)
	int sign = -1;
};
