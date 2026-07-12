#pragma once
#include <Entities/Enemies/Enemy.h>

class ZombieInfo : public WalkingEnemyInfo {
public:
	ZombieInfo(std::string name, int HP, int dmg, int defense, float jump_V0, float walk_V, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		WalkingEnemyInfo(isZombie, name, HP, dmg, defense, jump_V0, walk_V, hitboxSize, spriteSize) {}
	uint32_t* get_sprites_ids() override { return sprites_ids; }
private:
	uint32_t sprites_ids[6]{}; //6 sprites for all zombies probably
};

class Zombie : public WalkingEntity {
public:
	Zombie(int entity_id, float spawn_center_x, float spawn_center_y, EntityInfo& zombieInfo) :
		WalkingEntity(entity_id, spawn_center_x, spawn_center_y, zombieInfo.get_HP(), zombieInfo.get_defense(),
			zombieInfo.hitboxSize, zombieInfo.spriteSize) {
		physics.jump_V0 = zombieInfo.get_jump_V0();
		physics.walk_V = zombieInfo.get_walk_V();
		physics.current_Y_max_level = hitbox.center.y - hitbox.size.y * 0.5f; //mmm
		physics.Xinc = physics.walk_V;
	}
	void update_entity(float deltaTime) override {
		//manage zombie sprites
		sprite_time += deltaTime;
		sounds_cd_time += deltaTime;
		update_hit_cd(deltaTime);
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
			looks_at_left = false;
		}
		else {
			if (player_right_side) {
				player_right_side = !player_right_side;
				sign = -1;
			}
			physics.Xinc = -physics.walk_V * deltaTime;
			looks_at_left = true;
		}
	}
	void do_entity_hit_sound(std::unique_ptr<AudioManager>& audioManager) {
		audioManager->play_Sound(6);
	}
	void do_entity_death_sound(std::unique_ptr<AudioManager>& audioManager) {
		audioManager->play_Sound(8);
	}
	void do_entity_sounds(std::unique_ptr<AudioManager>& audioManager) {
		if (sounds_cd_time >= 5.f) {
			sounds_cd_time = 0.f;
			audioManager->play_Sound(9 + rand() % 2);
		}
	}
	void update_model() override {
		sprite_center_point.x = hitbox.center.x;
		sprite_center_point.y = hitbox.center.y - hitbox.size.y * 0.5f + sprite_size.y * 0.5f;
		matModel = glm::translate(glm::mat4(1.f), glm::vec3(sprite_center_point.x - sprite_size.x * 0.5f, sprite_center_point.y - sprite_size.y * 0.5f, 0.f));
		matModel = glm::scale(matModel, glm::vec3(sprite_size.x, sprite_size.y, 0.f));
	}
	int get_tex_index() const override {
		return current_sprite;
	}
private:
	float sprite_time = 0.f;
	float sounds_cd_time = 0.f; //cooldown for making sounds
	float time_standing = 0.f; //time when zombie is not moving right or left
	int current_sprite = 0; //current sprite of zombie
	bool player_right_side = false; //goes to the right or left side of player (used to prevent zombie from moving right and left on one place when at the same X position as player)
	int sign = -1;
};
