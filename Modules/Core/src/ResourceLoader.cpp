#include "Core/ResourceLoader.h"
#include <Core/System/Craft.h>
#include <Audio/AudioSystem.h>
#include <Resources/FontManager.h>
#include <Resources/TextureArrayMgr.h>
#include <Resources/Light.h>
#include <Resources/Effect.h>
#include <Resources/CoreTags.h>
#include <Resources/TagRegistry.h>
#include <Resources/ModsManager.h>
#include <Resources/Particle.h>
#include <Resources/AnimationClip.h>
#include <Resources/Animator.h>
#include <Objects/ObjectRegistry.h>
#include <Objects/ObjectManager.h>
#include <Objects/ObjectFactory.h>
#include <Entities/EntityRegistry.h>
#include <Entities/EntityInfoManager.h>
#include <Entities/EntityInfoFactory.h>
#include <Entities/EntityFactory.h>
#include <DeviceSystem/DeviceContext.h>
#include <Utility/Logger.h>
#include <iostream>

ResourceLoader::~ResourceLoader() {
	Engine::Util::DynamicLibLoader lib_loader;
	for (auto& lib : library_handles) {
		lib_loader.unload_library(lib.handle);
	}
}

bool ResourceLoader::load_minimal_core_resources() {
	if (DEBUG)
		data_extension = ".json";
	else
		data_extension = ".data";

	Engine::Object::register_core_object_factories();
	Engine::Entity::register_core_entity_info_factories();
	Engine::Entity::register_core_entity_factories();
	Engine::Resource::Tag::register_core_tags();

	Engine::Resource::SpriteManager::Instance().setup();
	Engine::Resource::LightSourceManager::Instance().setup();
	Engine::Resource::ParticleManager::Instance().setup();
	Engine::Resource::EffectsManager::Instance().setup();
	Engine::Object::ObjectManager::Instance().setup();

	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Base Resources loading started\n";
	bool ok = true;
	if (ok) ok = load_base_fonts();
	if (ok) ok = load_base_textures();
	if (ok) ok = load_base_sprites();
	if (ok) ok = load_base_audio();

	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Base Resources loading finished\n\n";
	return ok;
}

void ResourceLoader::start_resources_loading() {
	load_status = LoadStatus::LOAD_NOT_READY;
	can_load_next_batch = false;
	should_load_textures = true;
	resource_loader = std::make_unique<std::thread>(&ResourceLoader::ResourcesLoader, this);
	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Game resources loading started\n";
}

void ResourceLoader::load_resources_from_queue() {
	if (should_load_textures) {
		const bool all_textures_are_loaded = Engine::Resource::TextureArrayManager::Instance().load_textures_from_queue();
		if (all_textures_are_loaded) {
			should_load_textures = false;
			notify_loader();
		}
	}
}

void ResourceLoader::stop_resources_loading() {
	resource_loader->join();
	resource_loader.reset();
	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Game resources loading finished\n\n";
}

ResourceLoader::LoadStatus ResourceLoader::get_loading_status() {
	return load_status;
}

void ResourceLoader::lock_resources_mutex() {
	resources_lock.lock();
}

void ResourceLoader::unlock_resources_mutex() {
	resources_lock.unlock();
}

void ResourceLoader::ResourcesLoader() {
	// if any loader returns failure --> explain Error with Logger and give a choice to exit, hot-reload or turn off broken mod
	bool ok = true;

	ok = Load_mods();
	//if (ok) ok = Load_libraries();

	// LOAD ORDER IS IMPORTANT (each category is loaded from base files first and then from all mods)
	// textures are loaded in queue and through main thread, sprites and audio are loaded using resources mutex
	// all other resources are loaded as usual
	if (ok) ok = Load_textures();
	Register_textures_as_sprites();
	if (ok) ok = Load_resources(ResourceType::R_SPRITE, "Resources/data/sprites_data", true);
	if (ok) ok = Load_resources(ResourceType::R_AUDIO, "Resources/data/audio_data", true);
	if (ok) ok = Load_resources(ResourceType::R_LIGHT, "Resources/data/lights_data", true);
	if (ok) ok = Load_resources(ResourceType::R_PARTICLE, "Resources/data/particles_data", true);
	if (ok) ok = Load_resources(ResourceType::R_EFFECT, "Resources/data/effects_data", true);
	if (ok) ok = Load_resources(ResourceType::R_OBJECT, "Resources/data/objects_data", true);
	if (ok) ok = Load_resources(ResourceType::R_CRAFT, "Resources/data/crafts_data", true);
	if (ok) ok = Load_resources(ResourceType::R_ANIM_CLIP, "Resources/data/anim_data/clips", true);
	if (ok) ok = Load_resources(ResourceType::R_ANIMATOR, "Resources/data/anim_data/animators", true);
	if (ok) ok = Load_resources(ResourceType::R_ENTITY, "Resources/data/entity_data", true);
	
	if (ok) ok = Resolve_objects_dependencies();
	if (ok) ok = Resolve_entities_dependencies();

	if (ok) {
		load_status = LoadStatus::LOAD_READY;
	}
	else {
		load_status = LoadStatus::LOAD_FAILED;
	}
}

void ResourceLoader::notify_loader() {
	{
		std::lock_guard lock(tex_load_mutex);
		can_load_next_batch = true;
	}
	tex_load_cv.notify_one();
}

void ResourceLoader::Hot_Reload() {
	//Engine::Resource::TextureArrayManager::Instance().clear
	Engine::Resource::SpriteManager::Instance().ClearData();
	Engine::Resource::LightSourceManager::Instance().ClearData();
	//Engine::Resource::ParticleManager::Instance().clear;
	Engine::Resource::EffectsManager::Instance().ClearData();
	Engine::Resource::AnimationClipManager::Instance().ClearData();
	Engine::Resource::AnimatorManager::Instance().ClearData();
	Engine::Object::ObjectManager::Instance().ClearData();
	Engine::Object::ObjectFactoryRegistry::Instance().ClearData();
	Engine::Entity::EntityInfoFactoryRegistry::Instance().ClearData();
	Engine::Entity::EntityFactoryRegistry::Instance().ClearData();
	Engine::Entity::EntityInfoManager::Instance().ClearData();
	Engine::Resource::Tag::TagRegistry::Instance().ClearData();
	Engine::Core::Craft::CraftManager::Instance().ClearData();
	ResourcesLoader();
}

bool ResourceLoader::load_base_fonts() {
	if (Engine::Resource::FontManager::Instance().load_font("Resources/fonts/verdana_SDF", "Core") == false) {
		Engine::Util::Logger::Log("Fatal Error: Couldn't load base font");
		return false;
	}
	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Base font loaded\n";
	return true;
}
bool ResourceLoader::load_base_textures() {
	std::string textures[] = {
		"White", "Gradient", "Gradient2", "UI_Button32p", "UI_Button64p", "UI_Button64p1", "UI_Button64p2",
		"UI_Frame0", "UI_Frame1", "UI_StoneWall", "UI_ButtonPlay", "UI_ButtonDelete", "UI_ButtonInfo"
	};
	for (auto& tex_name : textures) {
		std::string file = tex_name + ".png";
		std::filesystem::path tex_path = "Resources/base_data/textures/" + file;
		if (!std::filesystem::exists(tex_path)) {
			Engine::Util::Logger::Log("Fatal Error: Couldn't load base UI textures");
			return false;
		}
		std::string UID = "Core:" + tex_name;
		if (Engine::Resource::TextureArrayManager::Instance().load_texture(tex_path, UID) == false) {
			Engine::Util::Logger::Log("Fatal Error: Couldn't load Texture <" + UID + ">!");
			return false;
		}
	}
	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Base textures loaded\n";
	return true;
}
bool ResourceLoader::load_base_sprites() {
	std::filesystem::path path = "Resources/base_data/sprites_data";
	std::filesystem::path file = path / "base_sprites.json";
	if (!std::filesystem::exists(file)) {
		Engine::Util::Logger::Log("Fatal Error: Couldn't find base sprites meta");
		return false;
	}
	if (Load_resources(ResourceType::R_SPRITE, "Resources/base_data/sprites_data", false) == false) {
		Engine::Util::Logger::Log("Fatal Error: Couldn't load base sprites");
		return false;
	}
	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Base sprites loaded\n";
	return true;
}
bool ResourceLoader::load_base_audio() {
	std::filesystem::path resource_path = "Resources/base_data/audio_data";
	if (!std::filesystem::exists(resource_path / "base_audio.json")) {
		Engine::Util::Logger::Log("Fatal Error: Couldn't find base audio meta");
		return false;
	}
	if (Load_resources(ResourceType::R_AUDIO, "Resources/base_data/audio_data", false) == false) {
		Engine::Util::Logger::Log("Fatal Error: Couldn't load base audio");
		return false;
	}
	std::cout << "[" << DeviceContext::get_current_system_time() << "] ResourceLoader: Base audio loaded\n";
	return true;
}

bool ResourceLoader::Load_mods() {
	std::filesystem::path info_file = "mod.json";
	std::filesystem::path description_file = "description.txt";
	std::filesystem::path icon_file = "mod_icon.png";
	std::filesystem::path mods_path_entry = "Mods";

	if (!std::filesystem::exists(mods_path_entry)) return true;

	for (const auto& entry : std::filesystem::directory_iterator(mods_path_entry)) {
		if (entry.is_regular_file()) continue;
		std::filesystem::path mod_folder_path = entry.path();

		std::filesystem::path info_path = mod_folder_path / info_file;
		if (!std::filesystem::exists(info_path)) {
			Engine::Util::Logger::Log("Mod <" + mod_folder_path.filename().string() + "> has no <mod.json>!");
			return false;
		}
		std::filesystem::path desc_path = mod_folder_path / description_file;
		if (!std::filesystem::exists(desc_path)) {
			Engine::Util::Logger::Log("Mod <" + mod_folder_path.filename().string() + "> has no <description.txt>!");
			return false;
		}
		std::filesystem::path icon_path = mod_folder_path / icon_file;
		if (!std::filesystem::exists(icon_path)) {
			Engine::Util::Logger::Log("Mod <" + mod_folder_path.filename().string() + "> has no <mod_icon.png>!");
			return false;
		}
		std::string texture_UID = mod_folder_path.filename().string() + ":mod_icon";
		// add mod icon texture right here while loading mod info
		if (Engine::Resource::TextureArrayManager::Instance().add_texture_to_queue(icon_path, texture_UID) == false) {
			return false;
		}
		// also register this texture as regular sprite later
		tex_to_sprite_queue.emplace_back(texture_UID);

		Engine::Util::DataParser parser;
		if (parser.parse_JSON_format(info_path) == false) {
			return false;
		}
		if (Engine::Mods::ModsManager::Instance().add_mod(mod_folder_path, parser.node_root) == false) {
			return false;
		}
		return true;
	}
}

bool ResourceLoader::Load_libraries() {
	std::filesystem::path lib_path;
	std::string lib_ext;
	typedef void(*REGISTER_OBJECT_FACTORIES)(Engine::Object::ObjectFactoryRegistry * obj_registry);
	typedef void(*REGISTER_ENTITY_INFO_FACTORIES)(Engine::Entity::EntityInfoFactoryRegistry * entity_info_registry);
	typedef void(*REGISTER_ENTITY_FACTORIES)(Engine::Entity::EntityFactoryRegistry * entity_registry);
	Engine::Util::DynamicLibLoader lib_loader;
	void* lib_handle = nullptr;
	void* fun = nullptr;

#if defined(_WIN32)
	lib_ext = ".dll";
#else
	lib_ext = ".so";
#endif
	for (const auto& mod_info : Engine::Mods::ModsManager::Instance().get_mods()) {
		if (!mod_info.has_dll) continue;
		lib_path = mod_info.mod_folder_path / "Resources/libs";
		if (!std::filesystem::exists(lib_path)) continue;

		for (const auto& entry : std::filesystem::directory_iterator(lib_path)) {
			if (!entry.is_regular_file()) continue;
			std::filesystem::path file_path = entry.path();
			if (file_path.extension() != lib_ext) continue;

			lib_handle = lib_loader.load_library(file_path.string().c_str());
			if (!lib_handle) {
				Engine::Util::Logger::Log("Couldn't load lib <" + file_path.filename().string() +
					"> from mod <" + mod_info.mod_folder_path.filename().string() + ">!");
				return false;
			}
			library_handles.emplace_back().handle = lib_handle;

			fun = lib_loader.get(lib_handle, "REGISTER_OBJECT_FACTORIES");
			if (fun) {
				REGISTER_OBJECT_FACTORIES reg_fun = (REGISTER_OBJECT_FACTORIES)fun;
				reg_fun(&Engine::Object::ObjectFactoryRegistry::Instance());
			}
			fun = lib_loader.get(lib_handle, "REGISTER_ENTITY_INFO_FACTORIES");
			if (fun) {
				REGISTER_ENTITY_INFO_FACTORIES reg_fun = (REGISTER_ENTITY_INFO_FACTORIES)fun;
				reg_fun(&Engine::Entity::EntityInfoFactoryRegistry::Instance());
			}
			fun = lib_loader.get(lib_handle, "REGISTER_ENTITY_FACTORIES");
			if (fun) {
				REGISTER_ENTITY_FACTORIES reg_fun = (REGISTER_ENTITY_FACTORIES)fun;
				reg_fun(&Engine::Entity::EntityFactoryRegistry::Instance());
			}
		}
	}
}

bool ResourceLoader::Load_textures() {
	auto _load_textures = [](std::filesystem::path& textures_path, std::string& source_name) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(textures_path)) {
			if (!entry.is_regular_file()) continue;
			std::filesystem::path file_path = entry.path();
			if (file_path.extension() == ".png") {
				std::string texture_UID = source_name + ":" + file_path.filename().string();
				texture_UID = texture_UID.substr(0, texture_UID.find_last_of("."));
				if (Engine::Resource::TextureArrayManager::Instance().add_texture_to_queue(file_path, texture_UID) == false) {
					return false;
				}
			}
		}
		return true;
	};

	std::filesystem::path textures_path = "Resources/textures";
	std::string source_name = "Core";
	if (_load_textures(textures_path, source_name) == false) {
		return false;
	}
	for (const auto& mod_info : Engine::Mods::ModsManager::Instance().get_mods()) {
		if (!mod_info.enabled) continue;
		source_name = mod_info.mod_folder_path.filename().string();
		// find mod textures
		textures_path = mod_info.mod_folder_path / "Resources/textures";
		if (!std::filesystem::exists(textures_path)) continue;
		if (_load_textures(textures_path, source_name) == false) {
			return false;
		}
	}
	Engine::Resource::TextureArrayManager::Instance().mark_queue_ready();

	std::unique_lock lock(tex_load_mutex);
	tex_load_cv.wait(lock, [&] { return can_load_next_batch; });
	return true;
}

void ResourceLoader::Register_textures_as_sprites() {
	for (const auto& uid : tex_to_sprite_queue) {
		std::lock_guard lock(resources_mutex);
		Engine::Resource::TextureArrayManager::Instance().register_texture_as_regular_sprite(uid);
	}
	tex_to_sprite_queue = {};
}

bool ResourceLoader::Load_resources(ResourceType res_type, const std::string& resources_path, bool load_from_mods) {
	std::vector<ResourceLoadRequest> load_requests;
	std::filesystem::path path;
	auto& mods = Engine::Mods::ModsManager::Instance().get_mods();
	load_requests.reserve(mods.size() + 1);

	path = resources_path;
	// load game resources as simple if DEBUG mode or packed if RELEASE mode
	load_requests.emplace_back(ResourceLoadRequest{ path, DEBUG ? ResourceLoadRequest::SIMPLE : ResourceLoadRequest::PACKED });

	if (load_from_mods) {
		for (const auto& mod_info : mods) {
			if (!mod_info.enabled) continue;
			path = mod_info.mod_folder_path / resources_path;
			if (std::filesystem::exists(path)) {
				// load mod resources as simple/packed based on mod.json meta
				const bool load_simple = mod_info.load_mode == Engine::Mods::ModInfo::LOAD_SIMPLE;
				load_requests.emplace_back(ResourceLoadRequest{ path, load_simple ? ResourceLoadRequest::SIMPLE : ResourceLoadRequest::PACKED });
			}
		}
	}

	bool ok = true;
	for (const auto& r : load_requests) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(r.data_file_path)) {
			if (!entry.is_regular_file()) continue;
			std::filesystem::path data_file_path = entry.path();
			if (data_file_path.extension() != data_extension) continue;

			Engine::Util::DataParser parser;
			if (parser.parse_data(data_file_path, r.load_type == ResourceLoadRequest::SIMPLE) == false) {
				return false;
			}
			switch (res_type) {
			case ResourceType::R_TEXTURE:
				//
				break;
			case ResourceType::R_SPRITE:
				ok &= Engine::Resource::SpriteManager::Instance().load_sprites_from_data(data_file_path, parser, resources_mutex);
				break;
			case ResourceType::R_AUDIO: {
				// audio_data -> data -> Resources -> Resources/audio
				std::filesystem::path audio_folder_path = path.parent_path().parent_path() / "audio";
				ok &= Engine::Audio::AudioSystem::Instance().load_audio_from_data(audio_folder_path, data_file_path, parser, resources_mutex);
				break;
			}
			case ResourceType::R_LIGHT:
				ok &= Engine::Resource::LightSourceManager::Instance().load_lights_from_data(data_file_path, parser);
				break;
			case ResourceType::R_PARTICLE:
				ok &= Engine::Resource::ParticleManager::Instance().load_particles_from_data(data_file_path, parser);
				break;
			case ResourceType::R_EFFECT:
				ok &= Engine::Resource::EffectsManager::Instance().load_effects_from_data(data_file_path, parser);
				break;
			case ResourceType::R_OBJECT:
				object_data_parsers.emplace_back(std::move(parser));
				ok &= Engine::Object::ObjectManager::Instance().load_objects_from_data(data_file_path, object_data_parsers.back());
				break;
			case ResourceType::R_CRAFT:
				ok &= Engine::Core::Craft::CraftManager::Instance().load_crafts_from_data(data_file_path, parser);
				break;
			case ResourceType::R_ANIM_CLIP:
				ok &= Engine::Resource::AnimationClipManager::Instance().load_clips_from_data(data_file_path, parser);
				break;
			case ResourceType::R_ANIMATOR:
				ok &= Engine::Resource::AnimatorManager::Instance().load_animators_from_data(data_file_path, parser);
				break;
			case ResourceType::R_ENTITY:
				entity_data_parsers.emplace_back(std::move(parser));
				ok &= Engine::Entity::EntityInfoManager::Instance().load_entities_from_data(data_file_path, entity_data_parsers.back());
				break;
			}

			if (!ok) return false;
		}
	}

	return true;
}

bool ResourceLoader::Resolve_objects_dependencies() {
	uint16_t object_ID = 2;
	for (const auto& parser : object_data_parsers) {
		for (const auto& j : parser.node_root) {
			Engine::Object::ObjectManager::Instance().get_object_info(object_ID)->fill_dependent_data(j);
			object_ID++;
		}
	}
	object_data_parsers = {};
	return true;
}

bool ResourceLoader::Resolve_entities_dependencies() {
	uint16_t entity_ID = 0;
	for (const auto& parser : entity_data_parsers) {
		for (const auto& j : parser.node_root) {
			Engine::Entity::EntityInfoManager::Instance().get_entity_info(entity_ID)->fill_dependent_data(j);
			entity_ID++;
		}
	}
	entity_data_parsers = {};
	return true;
}