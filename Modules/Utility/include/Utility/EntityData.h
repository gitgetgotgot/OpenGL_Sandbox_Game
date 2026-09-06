#pragma once
#include <glm/ext/vector_float2.hpp>

class Transform {
public:
	glm::vec2 pos{ 0.0f };
	glm::vec2 scale{ 1.0f };
	float rotation_angle = 0.0f;
};