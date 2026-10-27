#include <Core/ResourceLoader.h>
#include <Audio/AudioSystem.h>
#include <Resources/FontManager.h>
#include <Resources/TextureArrayMgr.h>
#include <Resources/Light.h>
#include <Resources/Effect.h>
#include <Resources/ResourceErrors.h>
#include <Resources/CoreTags.h>
#include <Resources/TagRegistry.h>
#include <Resources/ModsManager.h>
#include <Objects/ObjectRegistry.h>
#include <Objects/ObjectManager.h>
#include <Objects/ObjectFactory.h>
#include <Objects/Crafting.h>
#include <Entities/EntityRegistry.h>
#include <Entities/EntityInfoManager.h>
#include <Entities/EntityInfoFactory.h>
#include <Entities/EntityFactory.h>
#include <IOSystem/SystemContext.h>
#include <iostream>

ResourceLoader::~ResourceLoader() {
	DynamicLibLoader lib_loader;
	for (auto& lib : library_handles) {
		lib_loader.unload_library(lib.handle);
	}
}

void ResourceLoader::load_minimal_core_resources() {
	if (DEBUG)
		data_extension = ".json";
	else
		data_extension = ".data";
	resources_root = RESOURCE_ROOT;

	Engine::Object::register_core_object_factories();
	Engine::Entity::register_core_entity_info_factories();
	Engine::Entity::register_core_entity_factories();
	Engine::Resource::Tag::register_core_tags();

	Engine::Resource::SpriteManager::Instance().setup();
	Engine::Resource::LightSourceManager::Instance().setup();
	//particles
	Engine::Resource::EffectsManager::Instance().setup();
	Engine::Object::ObjectManager::Instance().setup();

	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Base Resources loading started\n";
	// load base font
	Engine::Resource::FontManager::Instance().load_font("Resources/fonts/verdana_SDF", "Core");
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Base font loaded\n";

	// load base UI textures
	std::string textures[] = {
		"White.png", "Gradient.png", "Gradient2.png", "UI_Button32p.png", "UI_Button64p.png", "UI_Button64p1.png", "UI_Button64p2.png",
		"UI_Frame0.png", "UI_Frame1.png", "UI_StoneWall.png", "UI_ButtonPlay.png", "UI_ButtonDelete.png", "UI_ButtonInfo.png"
	};
	for (auto& tex : textures) {
		std::filesystem::path tex_path = resources_root / "Resources/BaseData/textures" / tex;
		if (!std::filesystem::exists(tex_path)) {
			throw std::runtime_error("Fatal Error: Couldn't load base UI textures");
		}
		std::string UID = "Core:" + tex_path.filename().string();
		Engine::Resource::TextureArrayManager::Instance().load_texture(tex_path, UID);
	}
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Base textures loaded\n";

	// load base sprites
	std::filesystem::path resource_path = resources_root / "Resources/BaseData/sprites";
	if (!std::filesystem::exists(resource_path / "base_sprites.json")) {
		throw std::runtime_error("Fatal Error: Couldn't load base sprites");
	}
	_load_sprites(resource_path);
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Base sprites loaded\n";

	// load base audio
	resource_path = resources_root / "Resources/BaseData/audio";
	if (!std::filesystem::exists(resource_path / "audio_data/base_audio.json")) {
		throw std::runtime_error("Fatal Error: Couldn't load base audio");
	}
	_load_audio(resource_path);
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Base audio loaded\n";
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Base Resources loading finished\n\n";
}

void ResourceLoader::start_resources_loading() {
	resources_are_loaded = false;
	can_load_next_batch = false;
	should_load_textures = true;
	resource_loader = std::make_unique<std::thread>(&ResourceLoader::Load_Resources, this);
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Game resources loading started\n";
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
	std::cout << "[" << SystemContext::get_current_system_time() << "] ResourceLoader: Game resources loading finished\n\n";
}

bool ResourceLoader::resources_loading_ready() {
	return resources_are_loaded;
}

void ResourceLoader::lock_resources_mutex() {
	resources_lock.lock();
}

void ResourceLoader::unlock_resources_mutex() {
	resources_lock.unlock();
}

void ResourceLoader::Load_Resources() {
	find_mods();
	//load_libraries();

	// LOAD ORDER IS IMPORTANT (each category is loaded from base files first and then from all mods)
	// textures are loaded in queue and through main thread, sprites and audio are loaded using resources mutex
	// all other resources are loaded straight in loader thread
	load_textures();
	load_resources(LoadResType::R_SPRITE, "Resources/data/sprites_data", true);
	load_resources(LoadResType::R_AUDIO, "Resources/audio", true);
	load_resources(LoadResType::R_LIGHT, "Resources/data/lights_data", true);
	load_resources(LoadResType::R_PARTICLE, "Resources/data/particles_data", true);
	load_resources(LoadResType::R_EFFECT, "Resources/data/effects_data", true);
	load_resources(LoadResType::R_OBJECT, "Resources/data/objects_data", true);
	load_resources(LoadResType::R_CRAFT, "Resources/data/crafts_data", true);
	load_resources(LoadResType::R_ANIM_CLIP, "Resources/data/anim_data/clips", true);
	load_resources(LoadResType::R_ANIMATOR, "Resources/data/anim_data/animators", true);
	load_resources(LoadResType::R_ENTITY, "Resources/data/entity_data", true);

	resolve_items_dependencies();
	resolve_entities_dependencies();
	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	resources_are_loaded = true;
}

void ResourceLoader::notify_loader() {
	{
		std::lock_guard lock(tex_load_mutex);
		can_load_next_batch = true;
	}
	tex_load_cv.notify_one();
}

void ResourceLoader::Hot_Reload() {
	//textures_array.reset();
	Engine::Resource::SpriteManager::Instance().ClearData();
	Engine::Resource::LightSourceManager::Instance().ClearData();
	//particles mgr
	Engine::Resource::EffectsManager::Instance().ClearData();
	Engine::Object::ObjectManager::Instance().ClearData();
	Engine::Object::ObjectFactoryRegistry::Instance().ClearData();
	CraftingSystem::get_instance().ClearData();
	Engine::Resource::AnimationClipManager::Instance().ClearData();
	Engine::Resource::AnimatorManager::Instance().ClearData();
	Engine::Entity::EntityInfoFactoryRegistry::Instance().ClearData();
	Engine::Entity::EntityFactoryRegistry::Instance().ClearData();
	Engine::Entity::EntityInfoManager::Instance().ClearData();
	Engine::Resource::Tag::TagRegistry::Instance().ClearData();
	Load_Resources();
}

void ResourceLoader::find_mods() const {
	std::filesystem::path info_file = "mod.json";
	std::filesystem::path description_file = "description.txt";
	std::filesystem::path mods_path_entry = resources_root / "Mods";
	if (!std::filesystem::exists(mods_path_entry)) return;
	for (const auto& entry : std::filesystem::directory_iterator(mods_path_entry)) {
		if (entry.is_regular_file()) continue;
		std::filesystem::path mod_folder_path = entry.path();

		std::filesystem::path info_path = entry / info_file;
		if (!std::filesystem::exists(info_path))
			throw std::runtime_error("Mod has no <mod.json> in " + mod_folder_path.string());

		DataParser parser;
		parser.parse_JSON_format(info_path);
		try {
			Engine::Mods::ModInfo& mod = Engine::Mods::ModsManager::Instance().add(parser.node_root);
			mod.mod_folder_path = mod_folder_path;
			std::filesystem::path desc_file = mod_folder_path / description_file;
			parser.read_text_file(desc_file, mod.description);
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Error in <" + info_path.string() + ">: " + e.what());
		}
	}
}

void ResourceLoader::load_libraries() {
	std::filesystem::path lib_path;
	std::string lib_ext;
	typedef void(*REGISTER_OBJECT_FACTORIES)(Engine::Object::ObjectFactoryRegistry * obj_registry);
	typedef void(*REGISTER_ENTITY_INFO_FACTORIES)(Engine::Entity::EntityInfoFactoryRegistry * entity_info_registry);
	typedef void(*REGISTER_ENTITY_FACTORIES)(Engine::Entity::EntityFactoryRegistry * entity_registry);
	DynamicLibLoader lib_loader;
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
				std::string msg = "Couldn't load lib <"; msg += file_path.string(); msg += ">";
				throw std::runtime_error(msg);
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

void ResourceLoader::load_textures() {
	auto _load_textures = [&](std::filesystem::path& textures_path, std::string& source_name) {
		for (const auto& entry : std::filesystem::directory_iterator(textures_path)) {
			if (!entry.is_regular_file()) continue;
			std::filesystem::path file_path = entry.path();
			if (file_path.extension() == ".png") {
				std::string texture_UID = source_name + ":" + file_path.filename().string();
				Engine::Resource::TextureArrayManager::Instance().add_texture_to_queue(file_path, texture_UID);
			}
		}
	};

	std::filesystem::path textures_path = resources_root / "Resources/textures";
	std::string source_name = "Core";
	_load_textures(textures_path, source_name);
	for (const auto& mod_info : Engine::Mods::ModsManager::Instance().get_mods()) {
		source_name = mod_info.mod_folder_path.filename().string();
		// find mod icon texture and add sprite for this icon
		textures_path = mod_info.mod_folder_path / "mod_icon.png";
		if (std::filesystem::exists(textures_path)) {
			std::string texture_UID = source_name + ":" + "mod_icon.png";
			Engine::Resource::TextureArrayManager::Instance().add_texture_to_queue(textures_path, texture_UID);
		}
		// find mod textures
		textures_path = mod_info.mod_folder_path / "Resources/textures";
		if (!std::filesystem::exists(textures_path)) continue;
		_load_textures(textures_path, source_name);
	}
	Engine::Resource::TextureArrayManager::Instance().mark_queue_ready();

	std::unique_lock lock(tex_load_mutex);
	tex_load_cv.wait(lock, [&] { return can_load_next_batch; });
}

void ResourceLoader::load_resources(LoadResType res_type, std::string resources_path, bool load_from_mods) {
	std::vector<std::filesystem::path> paths;
	std::filesystem::path path;
	auto& mods = Engine::Mods::ModsManager::Instance().get_mods();
	paths.reserve(mods.size() + 1);

	path = resources_root / resources_path;
	paths.emplace_back(path);
	if (load_from_mods) {
		for (const auto& mod_info : mods) {
			path = mod_info.mod_folder_path / resources_path;
			if (std::filesystem::exists(path)) paths.emplace_back(path);
		}
	}
	for (auto& p : paths) {
		switch (res_type) {
		case LoadResType::R_TEXTURE:

			break;
		case LoadResType::R_SPRITE:
			_load_sprites(p);
			break;
		case LoadResType::R_AUDIO:
			_load_audio(p);
			break;
		case LoadResType::R_LIGHT:
			_load_lights(p);
			break;
		case LoadResType::R_PARTICLE:
			_load_particles(p);
			break;
		case LoadResType::R_EFFECT:
			_load_effects(p);
			break;
		case LoadResType::R_OBJECT:
			_load_objects(p);
			break;
		case LoadResType::R_CRAFT:
			_load_crafts(p);
			break;
		case LoadResType::R_ANIM_CLIP:
			_load_clips(p);
			break;
		case LoadResType::R_ANIMATOR:
			_load_animators(p);
			break;
		case LoadResType::R_ENTITY:
			_load_entities(p);
			break;
		}
	}
}

void ResourceLoader::_load_sprites(std::filesystem::path& sprites_path) {
	for (const auto& entry : std::filesystem::directory_iterator(sprites_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& s : parser.node_root.array) {
			if (!s.has_child("UID"))
				throw std::runtime_error("Sprite entry in " + file_path.string() + " has no UID!");
			std::string UID = s["UID"].get_as<std::string>();
			try {
				std::string texture_UID = s["texture"].get_as<std::string>();
				uint32_t padding = s["padding"].get_as<uint32_t>();

				auto tex_id_opt = Engine::Resource::TextureArrayManager::Instance().get_texture_id(texture_UID);
				if (!tex_id_opt.has_value())
					Engine::Resource::throw_reference_error(UID, texture_UID);
				uint32_t tex_array_id = tex_id_opt.value().array_id;
				uint32_t tex_id = tex_id_opt.value().index;

				auto& sprite = s["sprite"];
				uint32_t u0 = sprite["U0"].get_as<uint32_t>();
				uint32_t v0 = sprite["V0"].get_as<uint32_t>();
				uint32_t w = sprite["w"].get_as<uint32_t>();
				uint32_t h = sprite["h"].get_as<uint32_t>();
				//9-sliced sprite
				if (sprite.has_child("border")) {
					auto& border = sprite["border"];
					uint32_t left = border["left"].get_as<uint32_t>();
					uint32_t right = border["right"].get_as<uint32_t>();
					uint32_t top = border["top"].get_as<uint32_t>();
					uint32_t bottom = border["bottom"].get_as<uint32_t>();
					{
						std::lock_guard lock(resources_mutex);
						Engine::Resource::SpriteManager::Instance().add_sprite9sliced(UID, u0, v0, w, h, left, right, top, bottom, tex_array_id, tex_id);
					}
				}
				//regular sprite
				else {
					float base_size = sprite["base_size"].get_as<float>();
					{
						std::lock_guard lock(resources_mutex);
						Engine::Resource::SpriteManager::Instance().add_sprite(UID, u0, v0, w, h, base_size, tex_array_id, tex_id);
					}
				}
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_audio(std::filesystem::path& audio_path) {
	std::filesystem::path audio_data_path = audio_path / "audio_data";
	for (const auto& entry : std::filesystem::directory_iterator(audio_data_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& audio : parser.node_root.array) {
			if (!audio.has_child("UID"))
				throw std::runtime_error("Audio entry in " + file_path.string() + " has no UID!");
			std::string UID = audio["UID"].get_as<std::string>();
			try {
				std::filesystem::path file_path = audio_path / audio["file"].get_as<std::string>();
				std::string sound_group = audio["group"].get_as<std::string>();
				bool load_as_PCM = audio["PCM"].get_as<bool>();
				uint8_t max_instance = audio["MAX_INSTANCE"].get_as<uint8_t>();
				if (load_as_PCM) {
					std::lock_guard lock(resources_mutex);
					Engine::Audio::AudioSystem::Instance().load_audio_as_pcm(file_path, UID, sound_group, max_instance);
				}
				else {
					std::lock_guard lock(resources_mutex);
					Engine::Audio::AudioSystem::Instance().load_audio_as_stream(file_path, UID, sound_group, max_instance);
				}
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_lights(std::filesystem::path& lights_path) const {
	for (const auto& entry : std::filesystem::directory_iterator(lights_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& light : parser.node_root.array) {
			if (!light.has_child("UID"))
				throw std::runtime_error("Light entry in " + file_path.string() + " has no UID!");
			std::string UID = light["UID"].get_as<std::string>();
			try {
				float radius = light["radius"].get_as<float>();
				float r = light["r"].get_as<float>();
				float g = light["g"].get_as<float>();
				float b = light["b"].get_as<float>();
				Engine::Resource::LightSourceManager::Instance().add_light(UID, radius, glm::vec3(r, g, b));
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_particles(std::filesystem::path& particles_path) const {
	//add empty particle with global_ID = 0
}

void ResourceLoader::_load_effects(std::filesystem::path& effects_path) const {
	for (const auto& entry : std::filesystem::directory_iterator(effects_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& effect : parser.node_root.array) {
			if (!effect.has_child("UID"))
				throw std::runtime_error("Effect entry in " + file_path.string() + " has no UID!");
			std::string UID = effect["UID"].get_as<std::string>();
			try {
				std::string sprite_UID = effect["sprite_UID"].get_as<std::string>();
				auto sprite_global_id = Engine::Resource::SpriteManager::Instance().get_sprite_id(sprite_UID);
				if (!sprite_global_id)
					Engine::Resource::throw_reference_error(UID, sprite_UID);
				uint8_t effect_type_id = effect["effect_type"].get_as<uint8_t>();
				uint8_t stat_type_id = effect["stat_type"].get_as<uint8_t>();
				float value = effect["value"].get_as<float>();

				uint32_t particle_id = 0;
				float p_spawn_cd = 0.0f, dmg_cd = 0.0f;
				if (effect.has_child("dmg_cd")) dmg_cd = effect["dmg_cd"].get_as<float>();
				if (effect.has_child("particle")) {
					auto& p = effect["particle"];

				}
				Engine::Resource::EffectsManager::Instance().add_effect_info(
					(Engine::Resource::Effect::EffectType)effect_type_id, UID,
					(Engine::Resource::Effect::EffectStatType)stat_type_id, value,
					sprite_global_id.value(), particle_id, p_spawn_cd, dmg_cd
				);
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_objects(std::filesystem::path& objects_path) {
	for (const auto& entry : std::filesystem::directory_iterator(objects_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser& parser = item_data_parsers.emplace_back();
		parser.parse_data(file_path, DEBUG);
		for (const auto& item : parser.node_root.array) {
			if (!item.has_child("class_UID"))
				throw std::runtime_error("Item entry in " + file_path.string() + " has no class UID!");
			std::string class_uid = item["class_UID"].get_as<std::string>();
			auto factory_id = Engine::Object::ObjectFactoryRegistry::Instance().get_factory_id(class_uid);
			if (!factory_id)
				throw std::runtime_error("Info Class " + class_uid + " from file " + file_path.string() + " not found!");
			Engine::Object::_ObjectFactoryI* factory = Engine::Object::ObjectFactoryRegistry::Instance().get_factory(factory_id.value());
			factory->add_object(item);
		}
	}
}

void ResourceLoader::_load_crafts(std::filesystem::path& crafts_path) const {
	for (const auto& entry : std::filesystem::directory_iterator(crafts_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& craft : parser.node_root.array) {
			if (!craft.has_child("result_UID"))
				throw std::runtime_error("Craft entry in " + file_path.string() + " has no result_UID!");
			std::string result_UID = craft["result_UID"].get_as<std::string>();
			try {
				auto item_info_id = Engine::Object::ObjectManager::Instance().get_object_id(result_UID);
				if (!item_info_id.has_value())
					throw std::runtime_error("Craft entry in " + file_path.string() + ": <" + result_UID + "> doesn't exist!");
				uint16_t item_id = item_info_id.value();
				uint16_t amount = craft["amount"].get_as<uint16_t>();
				uint16_t condition_id = craft["condition"].get_as<uint16_t>();

				auto& items = craft["items"].array;
				std::vector<CraftingPair> craft_pairs;
				craft_pairs.reserve(items.size());

				for (auto& i : items) {
					std::string item_UID = i["item_UID"].get_as<std::string>();
					auto item_info_ID = Engine::Object::ObjectManager::Instance().get_object_id(item_UID);
					if (!item_info_ID.has_value())
						Engine::Resource::throw_reference_error(result_UID, item_UID);
					uint16_t item_ID = item_info_ID.value();
					uint16_t item_amount = i["item_amount"].get_as<uint16_t>();
					craft_pairs.emplace_back((uint16_t)item_ID, item_amount);
				}
				CraftingSystem::get_instance().add(item_id, amount, CraftCondition(condition_id), std::move(craft_pairs));
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(result_UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_clips(std::filesystem::path& clips_path) const {
	for (const auto& entry : std::filesystem::directory_iterator(clips_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& clip_data : parser.node_root.array) {
			if (!clip_data.has_child("UID"))
				throw std::runtime_error("Clip entry in " + file_path.string() + " has no UID!");
			std::string UID = clip_data["UID"].get_as<std::string>();
			try {
				Engine::Resource::AnimationClip& clip = Engine::Resource::AnimationClipManager::Instance().add_clip(UID);

				bool loop = clip_data["loop"].get_as<bool>();
				float frame_time = clip_data["frame_time"].get_as<float>();
				auto& sprites = clip_data["sprites"].array;

				clip.looping = loop;
				uint32_t sprites_size = sprites.size();
				clip.sprites.reserve(sprites_size);
				clip.set_frame_time(frame_time);
				for (uint32_t i = 0; i < sprites_size; i++) {
					std::string sprite_UID = sprites[i].get_as<std::string>();
					auto sprite_global_id = Engine::Resource::SpriteManager::Instance().get_sprite_id(sprite_UID);
					if (!sprite_global_id.has_value())
						Engine::Resource::throw_reference_error(UID, sprite_UID);
					else clip.sprites.emplace_back(sprite_global_id.value());
				}
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_animators(std::filesystem::path& animators_path) const {
	for (const auto& entry : std::filesystem::directory_iterator(animators_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);
		for (const auto& animator_data : parser.node_root.array) {
			if (!animator_data.has_child("UID"))
				throw std::runtime_error("Animator entry in " + file_path.string() + " has no UID!");
			std::string UID = animator_data["UID"].get_as<std::string>();
			try {
				Engine::Resource::Animator& animator = Engine::Resource::AnimatorManager::Instance().add_animator(UID);
				auto& clips = animator_data["clips"].array;
				uint32_t clips_size = clips.size();
				animator.clips.reserve(clips_size);
				for (uint32_t i = 0; i < clips_size; i++) {
					std::string clip_UID = clips[i].get_as<std::string>();
					auto clip_id_opt = Engine::Resource::AnimationClipManager::Instance().get_clip_id(clip_UID);
					if (!clip_id_opt.has_value())
						Engine::Resource::throw_reference_error(UID, clip_UID);
					else animator.clips.emplace_back(clip_id_opt.value());
				}
			}
			catch (const std::exception& e) {
				Engine::Resource::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::_load_entities(std::filesystem::path& entities_path) {
	for (const auto& entry : std::filesystem::directory_iterator(entities_path)) {
		if (!entry.is_regular_file()) continue;
		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser& parser = entity_data_parsers.emplace_back();
		parser.parse_data(file_path, DEBUG);
		for (const auto& entity_data : parser.node_root.array) {
			if (!entity_data.has_child("info_class_UID"))
				throw std::runtime_error("Entity entry in " + file_path.string() + " has no info_class_UID!");
			std::string info_class_uid = entity_data["info_class_UID"].get_as<std::string>();
			auto factory_id = Engine::Entity::EntityInfoFactoryRegistry::Instance().get_factory_id(info_class_uid);
			if (!factory_id.has_value())
				throw std::runtime_error("Info Class " + info_class_uid + " from file " + file_path.string() + " not found!");
			Engine::Entity::EntityInfoFactoryI* factory = Engine::Entity::EntityInfoFactoryRegistry::Instance().get_factory(factory_id.value());
			factory->add_entity_info(entity_data);
		}
	}
}

void ResourceLoader::resolve_items_dependencies() {
	uint16_t object_ID = 2;
	for (const auto& parser : item_data_parsers) {
		for (const auto& data_node : parser.node_root.array) {
			Engine::Object::ObjectManager::Instance().get_object_info(object_ID)->fill_dependent_data(data_node);
			object_ID++;
		}
	}
	item_data_parsers = {};
}

void ResourceLoader::resolve_entities_dependencies() {
	uint16_t entity_ID = 0;
	for (const auto& parser : entity_data_parsers) {
		for (const auto& data_node : parser.node_root.array) {
			Engine::Entity::EntityInfoManager::Instance().get_entity_info(entity_ID)->fill_dependent_data(data_node);
			entity_ID++;
		}
	}
	entity_data_parsers = {};
}