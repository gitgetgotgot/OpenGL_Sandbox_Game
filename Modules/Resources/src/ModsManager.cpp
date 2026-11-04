#include "Resources/ModsManager.h"
#include <Utility/Logger.h>

void Engine::Mods::ModsManager::ClearData() {
	//mods = {};
	UID_storage = {};
}

bool Engine::Mods::ModsManager::add_mod(const std::filesystem::path& mod_folder_path, const nlohmann::json& mod_meta) {
	using DP = Engine::Util::DataParser;

	std::string UID, version, load_mode;
	bool has_dll, enabled;

	bool ok = true;
	ok = DP::get_json_value<std::string, DP::JsonValue::V_STRING>(UID, mod_meta, "mod_UID");
	ok = DP::get_json_value<std::string, DP::JsonValue::V_STRING>(version, mod_meta, "version");
	ok = DP::get_json_value<std::string, DP::JsonValue::V_STRING>(load_mode, mod_meta, "load_mode");
	ok = DP::get_json_value<bool, DP::JsonValue::V_BOOL>(has_dll, mod_meta, "has_dll");
	ok = DP::get_json_value<bool, DP::JsonValue::V_BOOL>(enabled, mod_meta, "enabled");
	if (!ok) {
		Engine::Util::Logger::Log("Mod <" + mod_folder_path.filename().string() + "> has invalid meta file!");
		return false;
	}

	std::string desc;
	std::filesystem::path desc_file = mod_folder_path / "description.txt";
	if (Engine::Util::DataParser::read_text_file(desc_file, desc) == false) {
		return false;
	}

	ModInfo& mod = mods.emplace_back();
	mod.UID = UID_storage.emplace_back(UID);
	mod.version = version;
	mod.description = std::move(desc);
	mod.mod_folder_path = mod_folder_path;
	mod.load_mode = load_mode == "simple" ? ModInfo::LOAD_SIMPLE : ModInfo::LOAD_PACKED;
	mod.has_dll = has_dll;
	mod.enabled = enabled;

	return true;
}

std::vector<Engine::Mods::ModInfo>& Engine::Mods::ModsManager::get_mods() {
	return mods;
}

Engine::Mods::ModInfo& Engine::Mods::ModsManager::get_mod(uint32_t id) {
	return mods[id];
}