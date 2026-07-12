#include <Entities/EntitySystem.h>
#include <Entities/Enemies/Slime.h>
#include <Entities/Enemies/Zombie.h>
#include <Entities/Enemies/FlyingEye.h>
#include <Entities/Projectile.h>

void EntitySystem::init() {
	entities.reserve(1000);

	entity_sp = std::make_unique<ShaderProgram>("Resources/shaders/sprites2.vert", "Resources/shaders/sprites.frag");

	entity_ssbo = std::make_unique<SSBO>();
	entity_ssbo->set_data(nullptr, sizeof(EntityRenderData) * MAX_ENTITIES_RENDER, GL_DYNAMIC_DRAW);
	
	GLfloat pos[] = {
		0.f, 0.f,
		0.f, 1.f,
		1.f, 1.f,
		1.f, 0.f
	};
	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	instance_vao = std::make_unique<VAO>();
	instance_vao->bind_VAO();
	instance_vbo = std::make_unique<VBO>(pos, sizeof(pos));
	instance_ebo = std::make_unique<EBO>(indices, sizeof(indices));
	instance_vao->link_Attribute(0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
	instance_vao->unbind_VAO();
	instance_vbo->unbind_VBO();
	instance_ebo->unbind_EBO();
}

void EntitySystem::update() {

}

void EntitySystem::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	renderer->renderInstancedData(entity_sp, instance_vao, instance_vbo, instance_ebo, 6U, 9U);
}

bool EntitySystem::spawn_entity(uint32_t id, glm::vec2 pos) {
	std::unique_ptr<EntityInfo>& entityInfo = EntityDB::entityInfo[id];

	if (entityInfo->type == EntityType::isEnemy) {
		switch (entityInfo->get_enemy_type()) {
		case EnemyType::isSlime: {
			entities.emplace_back(std::make_unique<Slime>(id, pos.x, pos.y, *entityInfo));
		}
		case EnemyType::isZombie: {
			entities.emplace_back(std::make_unique<Zombie>(id, pos.x, pos.y, *entityInfo));
		}
		case EnemyType::isFlyingEye: {
			entities.emplace_back(std::make_unique<FlyingEye>(id, pos.x, pos.y, *entityInfo));
		}
		}
	}
	else if (entityInfo->type == EntityType::isProjectile) {
		//entities.emplace_back(std::make_unique<GravityProjectile>(id, pos.x, pos.y, *entityInfo));
	}
	return true;
}

bool EntitySystem::spawn_projectile() {
	return false;
}