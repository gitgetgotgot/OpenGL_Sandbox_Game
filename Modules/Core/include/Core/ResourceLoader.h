#pragma once
#include <unordered_map>
#include <filesystem>
#include <string>
#include <cstdint>
#include <memory>
#include <Utility/DataParser.h>
#include <Utility/DynamicLib.h>

#if DEBUG_MODE
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

class ResourceLoader {
public:
	static ResourceLoader& Instance() {
		static ResourceLoader loader;
		return loader;
	}
	void load_minimal_core_resources();
	void Load_Resources();
	void Hot_Reload();
	void find_mods() const;
	void load_libraries();
	void load_textures() const;
	void load_sprites() const;
	void load_audio() const;
	void load_lights() const;
	void load_particles();
	void load_effects() const;
	void load_items_data( std::vector<DataParser>& item_data_parsers);
	void load_crafts() const;
	void load_animation_clips_data() const;
	void load_animation_animators_data() const;
	void load_entities_data(std::vector<DataParser>& entity_data_parsers) const;

	void resolve_items_dependencies(const std::vector<DataParser>& item_data_parsers) const;
	void resolve_entities_dependencies(const std::vector<DataParser>& entity_data_parsers) const;
private:
	ResourceLoader() {}
	~ResourceLoader();

	void _LOAD_SPRITES(std::filesystem::path& sprites_path) const;
	void _LOAD_AUDIO(std::filesystem::path& sprites_path) const;

	std::vector<LibraryHandle> library_handles;
	std::filesystem::path data_extension;
	std::filesystem::path resources_root;
};