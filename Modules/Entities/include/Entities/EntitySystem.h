#pragma once
#include <Entities/GameEntities.h>
#include <Entities/Player.h>

constexpr uint32_t MAX_ENTITIES_RENDER = 500;

struct EntityRenderData {
	glm::mat4 modelMatrix;
	glm::vec4 UV;
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
	bool spawn_entity(uint32_t id, glm::vec2 pos);
	bool spawn_projectile();
private:
	EntitySystem() {}
	~EntitySystem() {}
	std::unique_ptr<ShaderProgram> entity_sp;
	std::unique_ptr<SSBO> entity_ssbo;
	std::unique_ptr<VAO> instance_vao;
	std::unique_ptr<VBO> instance_vbo;
	std::unique_ptr<EBO> instance_ebo;
	std::vector<EntityRenderData> entity_render_buf;
	uint32_t current_buf_size = 0;
	std::vector<std::unique_ptr<EntityBase>> entities;
};
