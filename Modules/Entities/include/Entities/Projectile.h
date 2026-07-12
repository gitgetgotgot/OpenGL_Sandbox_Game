#pragma once

#include <Entities/GameEntities.h>

class ProjectileInfo : public EntityInfo {
public:
	ProjectileInfo(ProjectileType type, int ammo_item_id, int damage, int max_enemy_hits,
		bool gravity, glm::vec2 spriteSize, uint32_t effect_comp_id = 0, uint32_t light_comp_id = 0) :
		EntityInfo(isProjectile, "", glm::vec2(0.f, 0.f), spriteSize), type{ type }, ammo_item_id{ ammo_item_id },
		DMG{ damage }, max_enemy_hits{ max_enemy_hits }, affected_by_gravity{ gravity } {
		this->spriteSize = spriteSize;
		this->effect_comp_id = effect_comp_id;
		this->light_comp_id = light_comp_id;
	}
	int get_ammo_item_id() const override { return ammo_item_id; }
	uint32_t* get_sprites_ids() override { return &sprite_id; }
	int get_dmg() const override { return DMG; }
	int get_proj_max_enemy_hits() const override { return max_enemy_hits; }
	bool proj_uses_gravity() const override { return affected_by_gravity; }
private:
	ProjectileType type;
	int ammo_item_id; //if projectile can be an item
	int DMG;
	float enemy_hit_cd = 0.5f;
	int max_enemy_hits;
	bool affected_by_gravity;
	uint32_t sprite_id;
};

class GravityProjectile : public EntityBase {
public:
	GravityProjectile(int entity_id, float speedX, float speedY, float flying_angle, float center_x, float center_y, glm::vec2 spriteSize, int DMG, int max_enemy_hits, bool isCrit, float spriteAdjustmentAngle) :
		EntityBase(entity_id), speedX{ speedX }, speedY{ speedY }, flying_angle{ flying_angle }, DMG{ DMG }, isCrit{ isCrit }, max_enemy_hits_available{ max_enemy_hits }, sprite_adjustment_angle{ spriteAdjustmentAngle } {
		hitbox.center = { center_x, center_y };
		sprite_center_point = hitbox.center;
		this->sprite_size = spriteSize;
		if (spriteAdjustmentAngle > 0.f) Yadjustment = 0.2f;
		else Yadjustment = 0.8f;
	}
	void update_entity(float deltaTime) override {
		/*
		float dx, dy;
		//calculate dX
		dx = speedX * deltaTime;
		sprite_center_point.x += dx;
		//calculate current y and dY
		float currentY = sprite_center_point.y + speedY * deltaTime - 9.8f * (block_size / 40.f) * time_of_flight * time_of_flight / 2.f;
		dy = currentY - sprite_center_point.y;
		sprite_center_point.y = currentY;
		time_of_flight += deltaTime;
		//calculate new angle of arrow
		flying_angle = std::atan(dy / dx) + glm::radians(sprite_adjustment_angle);
		if (dx < 0.f)
			flying_angle += glm::radians(180.f);
		//sprite model matrix
		matModel = glm::mat4(1.f);
		matModel = glm::translate(matModel, glm::vec3(sprite_center_point.x, sprite_center_point.y, 0.f));
		matModel = glm::rotate(matModel, flying_angle, glm::vec3(0.f, 0.f, 1.f));
		matModel = glm::translate(matModel, glm::vec3(-block_size * 0.5f, -block_size * Yadjustment, 0.f));
		matModel = glm::scale(matModel, glm::vec3(block_size, block_size, 0.f));
		//hitbox
		hitbox.size = { block_size * sprite_size.x * 0.3f, block_size * sprite_size.x * 0.3f };
		hitbox.center = sprite_center_point;
		*/
	}
	void update_model() override {}
	int get_proj_dmg() const override { return DMG; }
	bool dmg_is_crit() const override { return isCrit; }
	bool update_proj_hits_counter() override {
		max_enemy_hits_available--;
		if (max_enemy_hits_available == 0) return false;
		return true;
	}
private:
	float speedX, speedY;
	float time_of_flight = 0.f;
	float flying_angle;
	int DMG;
	bool isCrit;
	int max_enemy_hits_available;
	float sprite_adjustment_angle;
	float Yadjustment;
};

class LinearProjectile : public EntityBase {
public:
	LinearProjectile(int entity_id, float speedX, float speedY, float flying_angle, float center_x, float center_y, glm::vec2 spriteSize, int DMG, int max_enemy_hits, bool isCrit) :
		EntityBase(entity_id), speedX{ speedX }, speedY{ speedY }, flying_angle{ flying_angle }, DMG{ DMG }, isCrit{ isCrit }, max_enemy_hits_available{ max_enemy_hits } {
		hitbox.center = { center_x, center_y };
		sprite_center_point = hitbox.center;
		this->sprite_size = spriteSize;
	}
	void update_entity(float deltaTime) override {
		/*
		//sprite
		hitbox.center.x += speedX * deltaTime;
		hitbox.center.y += speedY * deltaTime;
		sprite_center_point = hitbox.center;
		matModel = glm::mat4(1.f);
		matModel = glm::translate(matModel, glm::vec3(hitbox.center.x, hitbox.center.y, 0.f));
		matModel = glm::rotate(matModel, flying_angle, glm::vec3(0.f, 0.f, 1.f));
		matModel = glm::translate(matModel, glm::vec3(-block_size * sprite_size.x / 2, -block_size * sprite_size.y / 2, 0.f));
		matModel = glm::scale(matModel, glm::vec3(block_size * sprite_size.x, block_size * sprite_size.y, 0.f));
		//hitbox
		hitbox.size = { block_size * sprite_size.x * 0.3f, block_size * sprite_size.x * 0.3f };
		*/
	}
	void update_model() override {}
	int get_proj_dmg() const override { return DMG; }
	bool dmg_is_crit() const override { return isCrit; }
	bool update_proj_hits_counter() override {
		max_enemy_hits_available--;
		if (max_enemy_hits_available == 0) return false;
		return true;
	}
private:
	float speedX, speedY;
	float flying_angle;
	int DMG;
	bool isCrit;
	int max_enemy_hits_available;
};