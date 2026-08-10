#pragma once
#include <Entities/GameEntities.h>

class FlyingEye : public Mob {
	uint32_t flying_anim = 0;
public:
	FlyingEye(uint32_t entity_id, glm::vec2 spawn_center) : Mob(entity_id, spawn_center) {}
	void on_create() override;
	void on_destroy() override;
	void update() override;

	void update_entity(float deltaTime) {
		/*
		sprite_time += deltaTime;
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
		*/
	}
	
private:
	float flying_angle;
};
