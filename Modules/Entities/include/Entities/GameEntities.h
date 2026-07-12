#pragma once

#include <Audio/AudioManager.h>
#include <Entities/Effects.h>
#include <Physics/Colliders.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory>

enum EntityType : uint8_t {
	isEnemy, isNPC, isMob, isPet, isProjectile
};
enum EnemyType : uint8_t {
	isSlime, isZombie, isFlyingEye
};
enum ProjectileType : uint8_t {
	Arrow, Bullet, Throwable, MagicBall
};
enum EntityMovementType : uint8_t {
	isWalking, isSwimming, isFlying
};

//structure that contains information about drop from entity
struct DropInfo {
	uint32_t item_id = 0;
	float chance = 0.f;
	//amount of this item in range [amount1, amount2]
	uint32_t amount1 = 0;
	uint32_t amount2 = 0;
	DropInfo(uint32_t id, float chance, uint32_t amount) : item_id{ id }, chance{ chance }, amount1{ amount }, amount2{ amount } {}
	DropInfo(uint32_t id, float chance, uint32_t amount1, uint32_t amount2) : item_id{ id }, chance{ chance }, amount1{ amount1 }, amount2{ amount2 } {}
};

struct DroppedItem {
	bool has_pick_cd = false; //if player is throwing something, then it has cd for 1 second for picking it up
	float cd_time = 0.f;
	int id = 0;
	int amount = 0;
	Collider_2D_AABB hitbox;
	//physics
	bool has_bottom_collision = false;
	float fallingDistance = 0.f;
	float time_in_free_falling = 0.f;
	float Xinc = 0.f;
};

class EntityInfo {
public:
	EntityInfo(EntityType type, std::string name, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		type{ type }, name{ name }, hitboxSize{ hitboxSize }, spriteSize{ spriteSize } {}
	~EntityInfo() {}
	//enemies
	virtual int get_HP() const { return 0; }
	virtual int get_dmg() const { return 0; }
	virtual int get_defense() const { return 0; }
	virtual float get_jump_V0() const { return 0; }
	virtual float get_walk_V() const { return 0; }
	virtual float get_moving_X_V() const { return 0; }
	virtual float get_moving_Y_V() const { return 0; }
	virtual EnemyType get_enemy_type() { return isSlime; }
	virtual std::vector<DropInfo>& get_drop_v() { std::vector<DropInfo> d; return d; }
	//projectiles
	virtual int get_ammo_item_id() const { return -1; }
	virtual uint32_t* get_sprites_ids() { return nullptr; }
	virtual int get_proj_max_enemy_hits() const { return 0; }
	virtual bool proj_uses_gravity() const { return false; }
	EntityType type;
	std::string name;
	glm::vec2 hitboxSize;
	glm::vec2 spriteSize;
	//usable components (if ID = 0 then no component is used)
	uint32_t effect_comp_id = 0;
	uint32_t light_comp_id = 0;
};

class EntityDB {
public:
	//entities info: 7 projectiles, 1 zombie, 1 slime, 3 eyes
	inline static std::unique_ptr<EntityInfo> entityInfo[15];
};

class NPC_Info : public EntityInfo {
public:

private:
	int damage;
};

class MobInfo : public EntityInfo {
public:
	int HP;
private:
	int damage;
};

class PetInfo : public EntityInfo {
public:

private:

};

struct WalkingEnemyPhysics {
	bool has_bottom_collision = false;
	bool has_side_collision = false;
	bool has_top_collision = false;
	bool moving_down = false;
	bool has_bottom_collision_only_with_objects = false; //check if entity has bottom collision with objects that have "bottom collision only"
	bool should_jump = false;
	float fallingDistance = 0.f; //to calculate the increment for moving on Y axis
	float time_in_free_falling = 0.f;
	float jump_V0, current_jump_V = 0.f;
	float walk_V;
	float Xinc = 0.f;
	int current_Y_max_level; //Y level of current block layer, where the entity is standing or stopped going up from jump
};
struct FlyingEnemyPhysics {
	bool has_bottom_collision = false;
	bool has_side_collision = false;
	bool has_top_collision = false;
	bool moving_down = false;
	float flyingX_V, flyingY_V;
	float Xinc = 0.f, Yinc = 0.f;
};

struct EntityStats {
	std::vector<Effects::Effect> effects;
	bool applyEffect(Effects::Effect& effect) {}

	void inflictDamage(float dmg) {
		HP -= dmg;
	}

	void removeEffect(Effects::Effect& effect) {}

	int HP = 100;
	int DEF = 0;
	float speedFactor = 1.f;
	float hit_cd = 0.f; //cd after receiving hit from melee weapon
};

class EntityBase {
public:
	EntityBase(int entity_id) : entity_id{ entity_id } {}
	~EntityBase() {}
	virtual void update_entity(float deltaTime) = 0;
	virtual void update_model() = 0;
	virtual void do_entity_sounds(std::unique_ptr<AudioManager>& audioManager) {}
	virtual void do_entity_hit_sound(std::unique_ptr<AudioManager>& audioManager) {}
	virtual void do_entity_death_sound(std::unique_ptr<AudioManager>& audioManager) {}

	virtual bool update_proj_hits_counter() { return true; }

	virtual WalkingEnemyPhysics& get_walking_physics() { WalkingEnemyPhysics p; return p; }
	virtual FlyingEnemyPhysics& get_flying_physics() { FlyingEnemyPhysics p; return p; }
	virtual int get_HP() const { return 0; }
	virtual void decrease_HP(int value) {};
	virtual int get_tex_index() const { return 0; }
	virtual int get_proj_dmg() const { return 0; }
	virtual bool dmg_is_crit() const { return false; }
	virtual EntityMovementType get_movement_type() const { return isWalking; }
	virtual EntityStats& get_entity_stats() { EntityStats s; return s; }
	int entity_id;
	bool looks_at_left = false;
	glm::mat4 matModel;
	glm::vec2 sprite_center_point;
	glm::vec2 sprite_size;
	Collider_2D_AABB hitbox;
};

class LivingEntity : public EntityBase {
public:
	LivingEntity(int entity_id, float spawn_center_x, float spawn_center_y, int HP, int DEF, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		EntityBase(entity_id) {
		stats.HP = HP; stats.DEF = DEF;
		sprite_size = spriteSize;
		hitbox.center = { spawn_center_x, spawn_center_y };
		hitbox.size = hitboxSize;
	}
	int get_HP() const override { return stats.HP; }
	void decrease_HP(int value) override { stats.HP -= value; }
	EntityStats& get_entity_stats() override { return stats; }
	void update_hit_cd(float deltaTime) {
		if (stats.hit_cd > 0.f) {
			stats.hit_cd -= deltaTime;
			if (stats.hit_cd <= 0.f)
				stats.hit_cd = 0.f;
		}
	}
protected:
	EntityStats stats;
};
class WalkingEntity : public LivingEntity {
public:
	WalkingEntity(int entity_id, float spawn_center_x, float spawn_center_y, int HP, int DEF, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		LivingEntity(entity_id, spawn_center_x, spawn_center_y, HP, DEF, hitboxSize, spriteSize) {}
	WalkingEnemyPhysics& get_walking_physics() override { return physics; }
	EntityMovementType get_movement_type() const override { return EntityMovementType::isWalking; }
protected:
	WalkingEnemyPhysics physics;
};
class FlyingEntity : public LivingEntity {
public:
	FlyingEntity(int entity_id, float spawn_center_x, float spawn_center_y, int HP, int DEF, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		LivingEntity(entity_id, spawn_center_x, spawn_center_y, HP, DEF, hitboxSize, spriteSize) {}
	FlyingEnemyPhysics& get_flying_physics() override { return physics; }
	EntityMovementType get_movement_type() const override { return EntityMovementType::isFlying; }
protected:
	FlyingEnemyPhysics physics;
};
