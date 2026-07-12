#pragma once
#include <Utility/Math.h>
#include <Utility/Sprite.h>
#include <Utility/GameContext.h>
#include <Utility/AppliableComps.h>
#include <Rendering/OpenGL_Renderer.h>
#include <glm/glm.hpp>

struct ParticleInfo {
	uint32_t sprite_id = 0;
	uint32_t light_id = 0;
};

class ParticlesManager {
public:
	static ParticlesManager* get_instance() {
		static ParticlesManager pMgr;
		return &pMgr;
	}
	void add_particle_info(uint32_t sprite_id, uint32_t light_id) {
		particles_info.emplace_back(ParticleInfo{ sprite_id, light_id });
	}
	ParticleInfo& get_particle_info(uint32_t id) {
		return particles_info[id];
	}
private:
	ParticlesManager(){}
	~ParticlesManager(){}
	std::vector<ParticleInfo> particles_info;
};

class Particle {
public:
	Particle(uint32_t id, float lifeTime, glm::vec2 sprite_center, glm::vec2 sprite_size, glm::vec2 velocity) :
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
	static ParticleSystem* get_instance() {
		static ParticleSystem system;
		return &system;
	}
	void init();
	void emit_particle(uint32_t particle_id, glm::vec2 emitter_center, glm::vec2 emitter_size);
	void update();
	void render(std::unique_ptr<OpenGL_Renderer>& renderer);
private:
	ParticleSystem(){}
	~ParticleSystem(){}
	ParticlesManager* particlesMgr = nullptr;
	std::vector<Particle> particles;
};
