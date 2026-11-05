#pragma once
#include <unordered_map>
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
public:
	enum ResourceType : uint8_t {
		R_TEXTURE, R_SPRITE, R_AUDIO, R_LIGHT, R_PARTICLE, R_EFFECT, R_OBJECT, R_CRAFT, R_ANIM_CLIP, R_ANIMATOR, R_ENTITY
	};
	enum LoadStatus : uint8_t {
		LOAD_NOT_READY, LOAD_READY, LOAD_FAILED, LOAD_FATAL_ERROR
	};
public:
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

	void ResourcesLoader();
	void notify_loader();
	bool Load_mods();
	bool Load_libraries();
	bool Load_textures(); // textures are loaded in their own method, using load queue in main thread (OpenGL specific)
	void Register_textures_as_sprites(); // mod icons should be registered as regular sprites after all textures are loaded
	bool Load_resources(
		ResourceType res_type,
		const std::string& resources_path,
		bool load_from_mods
	);
	bool Resolve_objects_dependencies();
	bool Resolve_entities_dependencies();
private:
	ResourceLoader() {}
	~ResourceLoader();
private:
	// BATCH_LOAD_SIMPLE --> load standard files (.json | .png | etc.)
	// BATCH_LOAD_PACKED --> load packed files (.data | .pack | etc.)
	struct ResourceLoadRequest {
		enum RequestLoadType : uint8_t { SIMPLE, PACKED };
		std::filesystem::path data_file_path;
		RequestLoadType load_type;
	};
private:
	// items and entities data is cached when loading
	std::vector<Engine::Util::DataParser> object_data_parsers;
	std::vector<Engine::Util::DataParser> entity_data_parsers;

	std::vector<Engine::Util::LibraryHandle> library_handles;
	std::filesystem::path data_extension;

	std::vector<std::string> tex_to_sprite_queue;

	std::unique_ptr<std::thread> resource_loader;
	std::mutex resources_mutex;
	std::unique_lock<std::mutex> resources_lock{ resources_mutex, std::defer_lock };
	std::mutex tex_load_mutex;
	std::condition_variable tex_load_cv;
	std::atomic<LoadStatus> load_status = LoadStatus::LOAD_NOT_READY;
	bool can_load_next_batch = false;
	bool should_load_textures = true;
};