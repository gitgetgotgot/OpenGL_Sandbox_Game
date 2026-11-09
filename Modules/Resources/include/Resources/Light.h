#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <glm/ext/vector_float3.hpp>

namespace Engine::Util {
	class DataParser;
}

namespace Engine::Resource {
	struct LightSource {
		glm::vec3 light_color{ 0.f, 0.f, 0.f };
		float light_radius = 0.f; //measured in blocks
		LightSource() {}
		LightSource(float radius, glm::vec3 color) : light_radius{ radius }, light_color{ color } {}
	};

	class LightSourceManager {
	public:
		static LightSourceManager& Instance() {
			static LightSourceManager compsMgr;
			return compsMgr;
		}
		void setup();
		void ClearData();
		bool load_lights_from_data(
			const std::filesystem::path& data_file_path,
			const Engine::Util::DataParser& parser
		);
		void add_light(const std::string& uid, float radius, glm::vec3 color);
		std::optional<uint32_t> get_light_id(const std::string& uid);
		LightSource& get_light(uint32_t id);
	private:
		LightSourceManager() {}
		~LightSourceManager() {}
		std::vector<LightSource> lights;
		std::unordered_map<std::string, uint32_t> UID_to_ID;
	};
}