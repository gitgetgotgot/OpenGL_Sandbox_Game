#pragma once
#include <Entities/GameEntities.h>
#include <Entities/Player.h>

namespace GameEntity {
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
		static EntitySystem* get_instance() {
			static EntitySystem entityMgr;
			return &entityMgr;
		}
		void init();
		void update();
		void render(std::unique_ptr<OpenGL_Renderer>& renderer);
		void set_world_data(
			WorldSlot* world_slots_ptr, uint32_t world_width, uint32_t world_height,
			std::unordered_map<uint32_t, std::unique_ptr<ObjectComponent>>* obj_comps_ptr
		);
		bool spawn_entity(uint32_t id, glm::vec2 pos);
		bool spawn_projectile();
	private:
		EntitySystem() {}
		~EntitySystem() {}

		WorldSlot* world_slots_ptr = nullptr;
		uint32_t world_width = 0, world_height = 0;
		std::unordered_map<uint32_t, std::unique_ptr<ObjectComponent>>* obj_comps_ptr;

		std::unique_ptr<ShaderProgram> entity_sp;
		std::unique_ptr<SSBO> entity_ssbo;
		std::unique_ptr<VAO> instance_vao;
		std::unique_ptr<VBO> instance_vbo;
		std::unique_ptr<EBO> instance_ebo;

		std::vector<EntityRenderData> entity_render_buf;
		std::vector<std::unique_ptr<EntityBase>> entities;
	};
}
