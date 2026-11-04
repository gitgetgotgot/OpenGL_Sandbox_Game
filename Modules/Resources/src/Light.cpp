#include "Resources/Light.h"
#include <Utility/Logger.h>

void Engine::Resource::LightSourceManager::setup() {
	//add empty light with global_ID = 0
	lights.emplace_back();
	light_UID_to_ID["Core:Empty"] = 0;
}

void Engine::Resource::LightSourceManager::ClearData() {
	lights = {};
	light_UID_to_ID = {};
}

bool Engine::Resource::LightSourceManager::load_lights_from_data(
	std::filesystem::path& data_file_path,
	Engine::Util::DataParser& parser
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		std::string UID;
		float radius, R, G, B;
		bool ok = true;

		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(UID, j, "UID");
		ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(radius, j, "radius");
		ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(R, j, "r");
		ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(G, j, "g");
		ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(B, j, "b");

		if (ok) {
			Engine::Resource::LightSourceManager::Instance().add_light(UID, radius, glm::vec3(R, G, B));
		}
		else {
			Engine::Util::Logger::Log("Invalid syntax for LightSource in <" + data_file_path.string() + ">!");
			return false;
		}
	}
	return true;
}

void Engine::Resource::LightSourceManager::add_light(std::string uid, float radius, glm::vec3 color) {
	light_UID_to_ID.emplace(uid, lights.size());
	lights.emplace_back(radius, color);
}

std::optional<uint32_t> Engine::Resource::LightSourceManager::get_light_id(std::string light_UID) {
	auto it = light_UID_to_ID.find(light_UID);
	if (it != light_UID_to_ID.end())
		return it->second;
	else
		return std::nullopt;
}

Engine::Resource::LightSource& Engine::Resource::LightSourceManager::get_light(uint32_t id) {
	return lights[id];
}