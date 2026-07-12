#pragma once
#include <Entities/Enemies/Enemy.h>

class FlyingEyeInfo : public FlyingEnemyInfo {
public:
	FlyingEyeInfo(std::string name, int HP, int dmg, int defense, float flyingX_V,
		float flyingY_V, glm::vec2 hitboxSize, glm::vec2 spriteSize) :
		FlyingEnemyInfo(isFlyingEye, name, HP, dmg, defense, flyingX_V, flyingY_V, hitboxSize, spriteSize) {}
	uint32_t* get_sprites_ids() override { return sprites_ids; }
private:
	uint32_t sprites_ids[2]{}; //2 sprites for all flying eyes probably
};

class FlyingEye : public FlyingEntity {
public:
	FlyingEye(int entity_id, float spawn_center_x, float spawn_center_y, EntityInfo& eyeInfo) :
		FlyingEntity(entity_id, spawn_center_x, spawn_center_y, eyeInfo.get_HP(), eyeInfo.get_defense(),
			eyeInfo.hitboxSize, eyeInfo.spriteSize) {
		physics.flyingX_V = eyeInfo.get_moving_X_V();
		physics.flyingY_V = eyeInfo.get_moving_Y_V();
	}
	void update_entity(float deltaTime) override {
		sprite_time += deltaTime;
		update_hit_cd(deltaTime);
		if (sprite_time >= 0.25f) {
			sprite_time = 0.f;
			current_sprite++;
			if (current_sprite > 1)
				current_sprite = 0;
		}
		//Xinc
		if (GameContext::PLAYER_LAST_POS.x - hitbox.center.x > 0.f) {
			physics.Xinc += physics.flyingX_V * deltaTime * 0.01;
			if (physics.Xinc > physics.flyingX_V * deltaTime) physics.Xinc = physics.flyingX_V * deltaTime;
		}
		else {
			physics.Xinc -= physics.flyingX_V * deltaTime * 0.01;
			if (physics.Xinc < -physics.flyingX_V * deltaTime) physics.Xinc = -physics.flyingX_V * deltaTime;
		}
		//Yinc
		if (GameContext::PLAYER_LAST_POS.y - hitbox.center.y > 0.f) {
			physics.Yinc += physics.flyingY_V * deltaTime * 0.01;
			if (physics.Yinc > physics.flyingY_V * deltaTime) physics.Yinc = physics.flyingY_V * deltaTime;
		}
		else {
			physics.Yinc -= physics.flyingY_V * deltaTime * 0.01;
			if (physics.Yinc < -physics.flyingY_V * deltaTime) physics.Yinc = -physics.flyingY_V * deltaTime;
		}
	}
	void do_entity_hit_sound(std::unique_ptr<AudioManager>& audioManager) {
		audioManager->play_Sound(6);
	}
	void do_entity_death_sound(std::unique_ptr<AudioManager>& audioManager) {
		audioManager->play_Sound(7);
	}
	void update_model() override {
		hitbox.size = { 1.0f, 1.0f };
		//here sprite_center_point is used as center point for matModel
		if (physics.Xinc == 0)
			flying_angle = 0;
		else
			flying_angle = std::atan(physics.Yinc / physics.Xinc);
		if (physics.Xinc > 0.f)
			flying_angle += glm::radians(180.f);

		matModel = glm::mat4(1.f);
		matModel = glm::translate(matModel, glm::vec3(hitbox.center.x, hitbox.center.y, 0.f));
		matModel = glm::rotate(matModel, flying_angle, glm::vec3(0.f, 0.f, 1.f));
		matModel = glm::translate(matModel, glm::vec3(-sprite_size.y * 0.75f, -sprite_size.y * 0.5f, 0.f));
		matModel = glm::scale(matModel, glm::vec3(sprite_size.x, sprite_size.y, 0.f));
	}
	int get_tex_index() const override {
		return current_sprite;
	}
private:
	float sprite_time = 0.f;
	int current_sprite = 0; //current sprite of eye
	float flying_angle;
};
