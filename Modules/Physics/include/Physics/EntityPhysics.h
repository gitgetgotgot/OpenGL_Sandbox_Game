#pragma once
#include <glm/ext/vector_float2.hpp>

class EntityPhysics {
public:
	inline static float MAX_FALL_SPEED = 20.0f;
	inline static float GRAVITY = 9.8f;

	glm::vec2 linear_velocity{ 0.0f };
	bool affected_by_gravity = true;
	float gravity_scale = 1.0f;
	uint8_t collision_mask = 0;
	bool platform_collision = false;

	float fallingDistance = 0.f; //to calculate the increment for moving on Y axis
	float time_falling = 0.f;
	int current_Y_max_level; //Y level of current block layer, where the entity is standing or stopped going up from jump
};