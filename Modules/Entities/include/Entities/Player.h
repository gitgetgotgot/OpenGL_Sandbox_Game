#pragma once

#include <Entities/Effects.h>
#include <Physics/Colliders.h>
#include <Objects/Inventory.h>

struct PlayerStats {
	std::vector<Effects::Effect> effects;
	void updateEffects(float deltaTime);
	void manage_effect(Effects::Effect& effect, bool apply);
	void inflictDamage(float dmg) { HP -= dmg; }

	float HP = 100;
	float MANA = 20;
	float DEF = 0;
	float currentHP = 0;
	float currentMANA = 20;
	float speedFactor = 1.f;
	float damageMultiplier = 1.f;
	float critChanceBuff = 0.f;
	float regeneration = 1.f;
	float damageReduction = 0.f; //in percentage
	bool hasPotionSickness = false;
};

struct Player {
	//stats
	PlayerStats stats;
	Inventory inventory;
	//hitbox and physics
	Collider_2D_AABB hitbox;
	glm::vec2 sprite_left_down_corner;
	glm::vec2 sprite_size;
	bool looks_at_left = true;
	bool has_bottom_collision = false;
	bool has_side_collision = false;
	bool has_top_collision = false;
	bool moving_down = false;
	bool has_bottom_collision_only_with_objects = false; //check if the player has bottom collision with objects that have "bottom collision only"
	float fallingDistance = 0.f; //to calculate the increment for moving on Y axis
	float time_in_free_falling = 0.f; //time spent while moving after jump
	float jump_V0 = 0.f; //the starting speed of jump
	float speed = 200.f; //standard speed - 5 blocks per second
	float jump_speed = 600.f; //standard jump speed
	int current_Y_max_level; //Y of current block layer, where the player is standing or stopped going up from jump
	//sprite
	uint32_t sprites_ids[6]{};
	uint32_t current_sprite = 0;
	float sprite_time = 0.f;
};