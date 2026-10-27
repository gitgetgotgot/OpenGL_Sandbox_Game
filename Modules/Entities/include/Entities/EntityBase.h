#pragma once
#include <Resources/Animation.h>
#include <Utility/EntityData.h>

namespace Engine::Entity {
	class EntityBase {
	public:
		EntityBase(uint32_t entity_id) : entity_id{ entity_id } {}
		virtual ~EntityBase() {}
		virtual void on_create() = 0;
		virtual void on_destroy() = 0;
		virtual void update() = 0;

		uint32_t entity_id = 0;
		bool flip_x = false;
		Transform transform;
		Engine::Resource::Sprite* current_sprite = nullptr;
	};
}