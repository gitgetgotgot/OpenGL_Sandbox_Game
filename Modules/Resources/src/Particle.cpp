#include "Resources/Particle.h"
#include <Utility/DataParser.h>

void Engine::Resource::ParticleManager::setup() {
	particles.emplace_back();
	UID_to_ID.emplace("Core:Empty", 0);
}

bool Engine::Resource::ParticleManager::load_particles_from_data(
	const std::filesystem::path& data_file_path,
	const Engine::Util::DataParser& parser
) {
	return true;
}

void Engine::Resource::ParticleManager::add_particle_info(const std::string& UID, uint16_t sprite_id, uint16_t light_id) {
	UID_to_ID.emplace(UID, particles.size());
	particles.emplace_back(ParticleInfo{ sprite_id, light_id });
}

Engine::Resource::ParticleInfo& Engine::Resource::ParticleManager::get_particle_info(uint32_t id) {
	return particles[id];
}

std::optional<uint16_t> Engine::Resource::ParticleManager::get_particle_info_id(const std::string& UID) const {
	if (auto it = UID_to_ID.find(UID); it != UID_to_ID.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}