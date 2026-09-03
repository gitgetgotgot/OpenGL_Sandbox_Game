#pragma once
#include <unordered_map>
#include <filesystem>
#include <string>
#include <cstdint>
#include <memory>
#include <Rendering/Texture3D.h>
#include <Utility/DataParser.h>

#if DEBUG_MODE
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

class ResourceLoader {
public:
	static ResourceLoader* get_instance() {
		static ResourceLoader loader;
		return &loader;
	}
	void Load_Resources();
	void Hot_Reload();
	void find_sources(std::vector<std::filesystem::path>& paths) const;
	void load_textures(const std::vector<std::filesystem::path>& mod_paths, std::unordered_map<std::string, uint32_t>& texture_layers);
	void load_sprites(const std::vector<std::filesystem::path>& mod_paths, std::unordered_map<std::string, uint32_t>& texture_layers) const;
	void load_lights(const std::vector<std::filesystem::path>& mod_paths) const;
	void load_particles(const std::vector<std::filesystem::path>& mod_paths);
	void load_effects(const std::vector<std::filesystem::path>& mod_paths) const;
	void load_items_data(const std::vector<std::filesystem::path>& mod_paths, std::vector<DataParser>& item_data_parsers);
	void load_crafts(const std::vector<std::filesystem::path>& mod_paths) const;
	void load_animation_clips_data(const std::vector<std::filesystem::path>& mod_paths) const;
	void load_animation_animators_data(const std::vector<std::filesystem::path>& mod_paths) const;
	void load_entities_data(const std::vector<std::filesystem::path>& mod_paths) const;

	void resolve_items_dependencies(const std::vector<DataParser>& item_data_parsers) const;
	void resolve_entities_dependencies();
private:
	ResourceLoader() {}
	~ResourceLoader() {}
	std::unique_ptr<Texture3D> textures_array;
	std::filesystem::path data_extension;
	std::filesystem::path resources_root;
};