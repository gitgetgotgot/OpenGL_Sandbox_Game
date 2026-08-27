#pragma once
#include <unordered_map>
#include <filesystem>
#include <string>
#include <cstdint>
#include <memory>
#include <Rendering/Texture3D.h>

#if DEBUG_MODE
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif
//constexpr std::string_view RESOURCE_DIR = RESOURCE_ROOT;

class ResourceLoader {
public:
	static ResourceLoader* get_instance() {
		static ResourceLoader loader;
		return &loader;
	}
	void Load_Resources();
	void load_textures(std::unordered_map<std::string, uint32_t>& texture_layers);
	void load_sprites(std::unordered_map<std::string, uint32_t>& texture_layers) const;
	void load_lights() const;
	void load_particles();
	void load_effects() const;
	void load_items_data() const;
	void load_crafts() const;
	void load_animations_data();
	void load_entities_data();

	void resolve_items_dependencies() const;
	void resolve_entities_dependencies();
private:
	ResourceLoader() {}
	~ResourceLoader() {}
	std::unique_ptr<Texture3D> textures_array;
	std::filesystem::path data_files_path;
	std::filesystem::path data_extension;
	std::filesystem::path resources_root;
};