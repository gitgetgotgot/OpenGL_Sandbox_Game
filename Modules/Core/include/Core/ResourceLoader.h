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

#ifdef DEBUG_MODE
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

class ResourceLoader {
	enum LoadResType : uint8_t {
		R_TEXTURE, R_SPRITE, R_AUDIO, R_LIGHT, R_PARTICLE, R_EFFECT, R_OBJECT, R_CRAFT, R_ANIM_CLIP, R_ANIMATOR, R_ENTITY
	};
public:
	enum LoadStatus : uint8_t { LOAD_NOT_READY, LOAD_READY, LOAD_FAILED, LOAD_FATAL_ERROR };
	static ResourceLoader& Instance() {
		static ResourceLoader loader;
		return loader;
	}
	bool load_minimal_core_resources();
	void start_resources_loading();
	void load_resources_from_queue();
	void stop_resources_loading();
	LoadStatus get_loading_status();
	void lock_resources_mutex();
	void unlock_resources_mutex();
	void Hot_Reload();
private:
	bool load_base_fonts();
	bool load_base_textures();
	bool load_base_sprites();
	bool load_base_audio();

	void Load_Resources();
	void notify_loader();
	bool find_mods() const;
	bool load_libraries();

	bool load_textures();
	bool load_resources(LoadResType res_type, std::string resources_path, bool load_from_mods);
	bool resolve_items_dependencies();
	bool resolve_entities_dependencies();
private:
	ResourceLoader() {}
	~ResourceLoader();

	// items and entities data is cached when loading
	std::vector<Engine::Util::DataParser> object_data_parsers;
	std::vector<Engine::Util::DataParser> entity_data_parsers;

	std::vector<Engine::Util::LibraryHandle> library_handles;
	std::filesystem::path data_extension;

	std::unique_ptr<std::thread> resource_loader;
	std::mutex resources_mutex;
	std::unique_lock<std::mutex> resources_lock{ resources_mutex, std::defer_lock };
	std::mutex tex_load_mutex;
	std::condition_variable tex_load_cv;
	std::atomic<LoadStatus> load_status = LoadStatus::LOAD_NOT_READY;
	bool can_load_next_batch = false;
	bool should_load_textures = true;
};