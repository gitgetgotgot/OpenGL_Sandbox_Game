#pragma once
#include <Entities/Enemies/Enemy.h>

class SlimeInfo : public WalkingEnemyInfo {
public:
	SlimeInfo(std::string name, int HP, int dmg, int defense, float jump_V0, float walk_V, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		WalkingEnemyInfo(isSlime, name, HP, dmg, defense, jump_V0, walk_V, hitboxSize, spriteSize) {}
	uint32_t* get_sprites_ids() override { return sprites_ids; }
private:
	uint32_t sprites_ids[2]{}; //2 sprites for all slimes probably
};

class Slime : public WalkingEntity {
public:
	Slime(int entity_id, float spawn_center_x, float spawn_center_y, EntityInfo& slimeInfo) :
		WalkingEntity(entity_id, spawn_center_x, spawn_center_y, slimeInfo.get_HP(), slimeInfo.get_defense(),
			slimeInfo.hitboxSize, slimeInfo.spriteSize) {
		physics.jump_V0 = slimeInfo.get_jump_V0();
		physics.walk_V = slimeInfo.get_walk_V();
		physics.current_Y_max_level = hitbox.center.y - hitbox.size.y * 0.5f; //mmm
	}
	void update_entity(float deltaTime) override;
	void do_entity_hit_sound(AudioManager*& audioManager) {
		audioManager->play_Sound(6);
	}
	void do_entity_death_sound(AudioManager*& audioManager) {
		audioManager->play_Sound(7);
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
	float time_standing = 0.f; //time when slime is not moving
	int current_sprite = 0; //current sprite is first or second
};