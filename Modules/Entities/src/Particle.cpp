#include <Entities/Particle.h>
#include <Utility/TimeManager.h>

bool Particle::update(float deltaTime) {
	currentTime += deltaTime;
	if (currentTime >= lifeTime) {
		return true; //return true if particle should be destroyed
	}
	sprite_center.x += velocity.x * deltaTime;
	sprite_center.y += velocity.y * deltaTime;
	sprite_size *= 0.99f; //decay value
	angle += 180.f * deltaTime; //180 degrees per second
	if (angle > 360.0f) angle -= 360.0f;

	GameMath::update_2D_TRS(modelMatrix, sprite_center, sprite_size, angle);
	return false;
}

void ParticleSystem::init() {
	particlesMgr = ParticlesManager::get_instance();
}

void ParticleSystem::emit_particle(uint32_t particle_id, glm::vec2 emitter_center, glm::vec2 emitter_size) {
	glm::vec2 center(emitter_center.x - emitter_size.x * 0.5f + float(rand() % int(emitter_size.x)),
		emitter_center.y - emitter_size.y * 0.5f + float(rand() % int(emitter_size.y))
	);
	particles.emplace_back(Particle(
		particle_id, 2.f, center,
		glm::vec2(0.5f, 0.5f),
		glm::vec2(0.f, 1.0f))
	);
}

void ParticleSystem::update() {
	for (int i = 0; i < particles.size(); i++) {
		Particle& particle = particles[i];
		ParticleInfo& info = particlesMgr->get_particle_info(particle.id);
		if (info.light_id > 0) {
			//update_lighting(particle.sprite_center, info.light.light_color, info.light.light_radius * BLOCK_VISIBLE_SIZE, false);
		}
		if (particle.update(TimeManager::deltaTime)) {
			particles.erase(particles.begin() + i);
			i--;
			continue;
		}
		if (info.light_id > 0) {
			//update_lighting(particle.sprite_center, info.light.light_color, info.light.light_radius * BLOCK_VISIBLE_SIZE, true);
		}
	}
}

void ParticleSystem::render(std::unique_ptr<OpenGL_Renderer>& renderer) {

}