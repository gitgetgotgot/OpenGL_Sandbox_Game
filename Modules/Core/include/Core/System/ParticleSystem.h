#pragma once
#include <Rendering/Interfaces/Renderer.h>
#include <glm/ext/matrix_float4x4.hpp>

class ParticleEntity {
public:
	ParticleEntity(uint32_t id, float lifeTime, glm::vec2 sprite_center, glm::vec2 sprite_size, glm::vec2 velocity) :
		id{ id }, lifeTime{ lifeTime }, sprite_center{ sprite_center }, sprite_size{ sprite_size }, velocity{ velocity } {
	}
	bool update(float deltaTime);

	uint32_t id;
	float lifeTime, currentTime = 0.f, angle = 0.f;
	glm::vec2 sprite_center;
	glm::vec2 sprite_size;
	glm::vec2 velocity;
	glm::mat4 modelMatrix;
};

class ParticleSystem {
public:
	static ParticleSystem* Instance() {
		static ParticleSystem system;
		return &system;
	}
	void init();
	void emit_particle(uint32_t particle_id, glm::vec2 emitter_center, glm::vec2 emitter_size);
	void update();
	void render(std::unique_ptr<Engine::Graphics::Renderer>& renderer);
private:
	ParticleSystem() {}
	~ParticleSystem() {}
	std::vector<ParticleEntity> particles;
};