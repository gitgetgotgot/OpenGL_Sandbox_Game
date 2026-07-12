#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Collider_2D_AABB {
	glm::vec2 center;
	glm::vec2 size;
};

struct Collider_2D_OBB {
	glm::vec2 center;
	glm::vec2 size;
	float angle;
	glm::mat4 transformMatrix;

	void update_transform();
};

enum CollisionType : uint8_t { NONE, LEFT, RIGHT, TOP, BOTTOM, CORNER };

namespace Collisions {

	bool checkCollisionAABB(Collider_2D_AABB& c1, Collider_2D_AABB& c2);

	CollisionType getTypeCollisionAABBwithBlock(Collider_2D_AABB& c1, int column, int line, float blockSize);

	//the first hitbox is main and return type is for it
	CollisionType getTypeCollisionAABB(Collider_2D_AABB& c1, Collider_2D_AABB& c2);

	void projectOnAxis(glm::vec2* pos, glm::vec2& axis, float& min, float& max);

	bool checkOverlapOnAxis(glm::vec2* verticesA, glm::vec2* verticesB, glm::vec2& axis);

	bool checkCollision_AABB_with_OBB(Collider_2D_AABB& c1, Collider_2D_OBB& c2);

	bool checkCollisionOBB(Collider_2D_OBB& c1, Collider_2D_OBB& c2);
};