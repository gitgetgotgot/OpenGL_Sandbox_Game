#pragma once
#include <Entities/GameEntities.h>
#include <Entities/Player.h>

constexpr uint32_t MAX_ENTITIES_RENDER = 1000;

struct EntityRenderData {
	glm::mat4 modelMatrix;
	glm::vec2 tex_coords[4];
	float color_value = 0.f; //if entity should always be lightened with it's own color, then color_value is 1.f (not used for now, just for padding)
	float padding[3]{};
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
	EntityRenderData entity_render_buf[MAX_ENTITIES_RENDER];
	uint32_t current_buf_size = 0;
	std::vector<std::unique_ptr<EntityBase>> entities;
};
