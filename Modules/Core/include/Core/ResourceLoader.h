#pragma once
#include <unordered_map>
#include <filesystem>
#include <string>
#include <cstdint>
#include <memory>
#include <Rendering/Texture3D.h>
#include <Utility/DataParser.h>
#include <Utility/DynamicLib.h>

#if DEBUG_MODE
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

class ResourceLoader {
public:
	static ResourceLoader& get_instance() {
		static ResourceLoader loader;
		return loader;
	}
	void Load_Resources();
	void Hot_Reload();
	void find_mods() const;
	void load_libraries();
	void load_textures(std::unordered_map<std::string, uint32_t>& texture_layers);
	void load_sprites(std::unordered_map<std::string, uint32_t>& texture_layers) const;
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
	std::vector<LibraryHandle> library_handles;
	std::unique_ptr<Texture3D> textures_array;
	std::filesystem::path data_extension;
	std::filesystem::path resources_root;
};