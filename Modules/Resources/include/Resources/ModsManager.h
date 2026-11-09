#pragma once
#include "Utility/SimpleArray.h"
#include "Utility/DataParser.h"
#include <deque>

namespace Engine::Mods {
	struct DependentMod {
		std::string UID;
		std::string version;
	};

	struct ModInfo {
		// simple -> load .json, .png, etc; packed -> load .data, .pack, etc.
		enum ModLoadMode : uint8_t { LOAD_SIMPLE, LOAD_PACKED };
		std::string_view UID;
		std::string version;
		std::string description;
		std::filesystem::path mod_folder_path;
		Engine::Util::SimpleArray<DependentMod> dependent_mods;
		ModLoadMode load_mode;
		bool has_dll;
		bool enabled;
	};

	class ModsManager {
	public:
		static ModsManager& Instance() {
			static ModsManager mgr;
			return mgr;
		}
		void ClearData();
		bool add_mod(const std::filesystem::path& mod_folder_path, const nlohmann::json& mod_meta);
		std::vector<ModInfo>& get_mods();
		ModInfo& get_mod(uint32_t id);
	private:
		std::deque<std::string> UID_storage;
		std::vector<ModInfo> mods;
	};
}