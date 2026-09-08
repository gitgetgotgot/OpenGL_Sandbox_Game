#pragma once
#include "Utility/DynamicArray.h"
#include "Utility/DataParser.h"
#include <deque>

struct DependentMod {
	std::string UID;
	std::string version;
};

struct ModInfo {
	std::string_view UID;
	std::string version;
	std::string description;
	std::filesystem::path mod_folder_path;
	DynamicArray<DependentMod> dependent_mods;
	bool has_dll;
};

class ModsManager {
public:
	static ModsManager& get_instance() {
		static ModsManager mgr;
		return mgr;
	}
	void ClearData();
	ModInfo& add(const DataNode& data);
	std::vector<ModInfo>& get_mods();
private:
	std::deque<std::string> UID_storage;
	std::vector<ModInfo> mods;
};