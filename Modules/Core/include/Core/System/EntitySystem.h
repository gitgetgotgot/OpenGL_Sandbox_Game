#pragma once
#include "Entities/Player.h"
#include "Entities/EntityBase.h"
#include <Rendering/Interfaces/Renderer.h>
#include <Objects/ObjectComponents.h>
#include "Core/World/WorldData.h"

namespace Engine::Core {
	constexpr uint32_t MAX_ENTITIES_RENDER = 500;

	struct EntityRenderData {
		glm::mat4 modelMatrix;
		glm::vec4 UV;
	};

	struct ColorVertex2f {
		glm::vec2 pos;
		glm::vec4 color;
	};

	class EntitySystem {
	public:
		static EntitySystem& Instance() {
			static EntitySystem sys;
			return sys;
		}
		void init();
		void uninit();
		void update();
		void render(std::unique_ptr<Engine::Graphics::Renderer>& renderer);
		void connect_world_data(
			WorldSlot* world_slots_ptr, uint32_t world_width, uint32_t world_height,
			std::unordered_map<uint32_t, std::unique_ptr<Engine::Object::ObjectComponent>>* obj_comps_ptr
		);
		bool spawn_entity(uint32_t id, glm::vec2 pos);
		bool spawn_projectile();
	private:
		EntitySystem() = default;
		~EntitySystem() = default;

		WorldSlot* world_slots_ptr = nullptr;
		uint32_t world_width = 0, world_height = 0;
		std::unordered_map<uint32_t, std::unique_ptr<Engine::Object::ObjectComponent>>* obj_comps_ptr;

		std::unique_ptr<Engine::Graphics::ShaderProgram> entity_shader;
		std::unique_ptr<Engine::Graphics::StorageBuffer> entity_sb;
		std::unique_ptr<Engine::Graphics::VertexLayout> instance_va;
		std::unique_ptr<Engine::Graphics::VertexBuffer> instance_vb;
		std::unique_ptr<Engine::Graphics::IndexBuffer> instance_ib;

		std::vector<EntityRenderData> entity_render_buf;
		std::vector<std::unique_ptr<Engine::Entity::EntityBase>> entities;
	};
}