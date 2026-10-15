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
		R_SPRITE, R_TEXTURE, R_AUDIO, R_ANIMATION
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
	void load_sprites();
	void load_audio();
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

	void _LOAD_SPRITES(std::filesystem::path& sprites_path);
	void _LOAD_AUDIO(std::filesystem::path& sprites_path);
private:
	ResourceLoader() {}
	~ResourceLoader();

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