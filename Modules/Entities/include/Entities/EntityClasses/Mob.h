#pragma once
#include "Entities/EntityBase.h"
#include <Resources/Effects.h>
#include <Physics/EntityPhysics.h>
#include <Physics/Colliders.h>

namespace CoreEntity {
	struct MobStats {
		bool applyEffect(CoreResource::Effect& effect) { return true; }
		void removeEffect(CoreResource::Effect& effect) {}

		int HP = 0;
		int DEF = 0;
		float DMG = 0.0f;
		glm::vec2 speed{ 0.0f };
		float speedFactor = 1.f;
		std::vector<CoreResource::Effect> effects;
	};

	class Mob : public EntityBase {
	public:
		Mob(uint32_t entity_id, glm::vec2 spawn_center);

		CoreAnimation::AnimatorController anim_controller;
		MobStats stats;
		EntityPhysics physics;
		Collider_2D_AABB hitbox;
	};
}