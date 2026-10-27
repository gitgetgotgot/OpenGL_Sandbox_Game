#pragma once
#include <unordered_map>
#include <filesystem>
#include <string>
#include <cstdint>
#include <memory>
#include <thread>
#include <mutex>
#include <Utility/DataParser.h>
#include <Utility/DynamicLib.h>

#if DEBUG_MODE
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

class ResourceLoader {
	enum LoadResType : uint8_t {
		R_TEXTURE, R_SPRITE, R_AUDIO, R_LIGHT, R_PARTICLE, R_EFFECT, R_OBJECT, R_CRAFT, R_ANIM_CLIP, R_ANIMATOR, R_ENTITY
	};
public:
	static ResourceLoader& Instance() {
		static ResourceLoader loader;
		return loader;
	}
	void load_minimal_core_resources();
	void start_resources_loading();
	void load_resources_from_queue();
	void stop_resources_loading();
	bool resources_loading_ready();
	void lock_resources_mutex();
	void unlock_resources_mutex();
	void Hot_Reload();
private:
	void Load_Resources();
	void notify_loader();
	void find_mods() const;
	void load_libraries();

	void load_textures();
	void load_resources(LoadResType res_type, std::string resources_path, bool load_from_mods);
	void _load_sprites(std::filesystem::path& file_path);
	void _load_audio(std::filesystem::path& file_path);
	void _load_lights(std::filesystem::path& lights_path) const;
	void _load_particles(std::filesystem::path& particles_path) const;
	void _load_effects(std::filesystem::path& effects_path) const;
	void _load_objects(std::filesystem::path& objects_path);
	void _load_crafts(std::filesystem::path& crafts_path) const;
	void _load_clips(std::filesystem::path& clips_path) const;
	void _load_animators(std::filesystem::path& animators_path) const;
	void _load_entities(std::filesystem::path& entities_path);

	void resolve_items_dependencies();
	void resolve_entities_dependencies();
private:
	ResourceLoader() {}
	~ResourceLoader();

	// items and entities data is cached when loading
	std::vector<DataParser> item_data_parsers;
	std::vector<DataParser> entity_data_parsers;

	std::vector<LibraryHandle> library_handles;
	std::filesystem::path data_extension;
	std::filesystem::path resources_root;

	std::unique_ptr<std::thread> resource_loader;
	std::mutex resources_mutex;
	std::unique_lock<std::mutex> resources_lock{ resources_mutex, std::defer_lock };
	std::mutex tex_load_mutex;
	std::condition_variable tex_load_cv;
	std::atomic_bool resources_are_loaded = false;
	bool can_load_next_batch = false;
	bool should_load_textures = true;
};