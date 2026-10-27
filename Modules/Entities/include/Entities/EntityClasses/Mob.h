#pragma once
#include "Entities/EntityBase.h"
#include <Resources/Effect.h>
#include <Physics/EntityPhysics.h>
#include <Physics/Colliders.h>

namespace Engine::Entity {
	struct MobStats {
		bool applyEffect(Engine::Resource::Effect& effect) { return true; }
		void removeEffect(Engine::Resource::Effect& effect) {}

		int HP = 0;
		int DEF = 0;
		float DMG = 0.0f;
		glm::vec2 speed{ 0.0f };
		float speedFactor = 1.f;
		std::vector<Engine::Resource::Effect> effects;
	};

	class Mob : public EntityBase {
	public:
		Mob(uint32_t entity_id, glm::vec2 spawn_center);

		Engine::Resource::AnimatorController anim_controller;
		MobStats stats;
		Engine::Physics::EntityPhysics physics;
		Engine::Physics::Collider_2D_AABB hitbox;
	};
}