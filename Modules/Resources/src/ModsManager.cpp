#include "Resources/ModsManager.h"

void ModsManager::ClearData() {
	//mods = {};
	UID_storage = {};
}

ModInfo& ModsManager::add(const DataNode& data) {
	ModInfo& mod = mods.emplace_back();
	std::string UID = data["mod_UID"].get_as<std::string>();
	std::string version = data["version"].get_as<std::string>();
	bool has_dll = data["has_dll"].get_as<bool>();
	bool enabled = data["enabled"].get_as<bool>();
	
	mod.UID = UID_storage.emplace_back(UID);
	mod.version = version;
	mod.has_dll = has_dll;
	mod.enabled = enabled;
	return mod;
}

std::vector<ModInfo>& ModsManager::get_mods() {
	return mods;
}

ModInfo& ModsManager::get_mod(uint32_t id) {
	return mods[id];
}