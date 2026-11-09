#pragma once
#include <unordered_map>
#include <optional>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>

namespace Engine::Util {
	class DataParser;
}

namespace Engine::Resource {
	struct ParticleInfo {
		uint16_t sprite_id = 0;
		uint16_t light_id = 0;
	};

	class ParticleManager {
	public:
		static ParticleManager& Instance() {
			static ParticleManager m;
			return m;
		}
		void setup();
		bool load_particles_from_data(
			const std::filesystem::path& data_file_path,
			const Engine::Util::DataParser& parser
		);
		void add_particle_info(const std::string& UID, uint16_t sprite_id, uint16_t light_id);
		ParticleInfo& get_particle_info(uint32_t id);
		std::optional<uint16_t> get_particle_info_id(const std::string& UID) const;
	private:
		ParticleManager() {}
		~ParticleManager() {}
		std::unordered_map<std::string, uint16_t> UID_to_ID;
		std::vector<ParticleInfo> particles;
	};
}