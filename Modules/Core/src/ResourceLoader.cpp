#include <Core/ResourceLoader.h>
#include <Utility/Lights.h>
#include <Utility/Effects.h>
#include <Utility/ResourceErrors.h>
#include <Utility/CoreTags.h>
#include <Utility/TagRegistry.h>
#include <Utility/ModsManager.h>
#include <Objects/ObjectRegistry.h>
#include <Objects/ObjectManager.h>
#include <Objects/ObjectFactory.h>
#include <Objects/Crafting.h>
#include <Entities/EntityRegistry.h>
#include <Entities/EntityInfoManager.h>
#include <Entities/EntityInfoFactory.h>
#include <Entities/EntityFactory.h>

ResourceLoader::~ResourceLoader() {
	DynamicLibLoader lib_loader;
	for (auto& lib : library_handles) {
		lib_loader.unload_library(lib.handle);
	}
}

void ResourceLoader::Load_Resources() {
	if (DEBUG)
		data_extension = ".json";
	else
		data_extension = ".DATA";
	resources_root = RESOURCE_ROOT;

	std::vector<DataParser> item_data_parsers;
	std::vector<DataParser> entity_data_parsers;
	std::unordered_map<std::string, uint32_t> texture_layers;

	find_mods();
	CoreObject::register_core_object_factories();
	CoreEntity::register_core_entity_info_factories();
	CoreEntity::register_core_entity_factories();
	CoreTags::register_core_tags();

	const float pixel_UV_size = 1.f / 512.f;
	CoreResource::SpriteManager::MAIN_PIXEL_UV_SIZE = pixel_UV_size;

	// LOAD ORDER IS IMPORTANT
	load_textures(texture_layers);
	load_sprites(texture_layers);
	load_lights();
	load_particles();
	load_effects();
	load_items_data(item_data_parsers);
	load_crafts();
	load_animation_clips_data();
	load_animation_animators_data();
	load_entities_data(entity_data_parsers);

	resolve_items_dependencies(item_data_parsers);
	resolve_entities_dependencies(entity_data_parsers);
}

void ResourceLoader::Hot_Reload() {
	textures_array.reset();
	CoreResource::SpriteManager::get_instance().ClearData();
	CoreResource::LightsManager::get_instance().ClearData();
	//particles mgr
	CoreResource::EffectsManager::get_instance().ClearData();
	CoreObject::ObjectManager::get_instance().ClearData();
	CoreObject::ObjectFactoryRegistry::get_instance().ClearData();
	CraftingSystem::get_instance().ClearData();
	CoreAnimation::AnimationClipManager::get_instance().ClearData();
	CoreAnimation::AnimatorManager::get_instance().ClearData();
	CoreEntity::EntityInfoFactoryRegistry::get_instance().ClearData();
	CoreEntity::EntityFactoryRegistry::get_instance().ClearData();
	CoreEntity::EntityInfoManager::get_instance().ClearData();
	TagRegistry::get_instance().ClearData();
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
			ModInfo& mod = ModsManager::get_instance().add(parser.node_root);
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
	typedef void(*REGISTER_OBJECT_FACTORIES)(CoreObject::ObjectFactoryRegistry * obj_registry);
	typedef void(*REGISTER_ENTITY_INFO_FACTORIES)(CoreEntity::EntityInfoFactoryRegistry * entity_info_registry);
	typedef void(*REGISTER_ENTITY_FACTORIES)(CoreEntity::EntityFactoryRegistry * entity_registry);
	DynamicLibLoader lib_loader;
	void* lib_handle = nullptr;
	void* fun = nullptr;

#if defined(_WIN32)
	lib_ext = ".dll";
#else
	lib_ext = ".so";
#endif
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
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
				reg_fun(&CoreObject::ObjectFactoryRegistry::get_instance());
			}
			fun = lib_loader.get(lib_handle, "REGISTER_ENTITY_INFO_FACTORIES");
			if (fun) {
				REGISTER_ENTITY_INFO_FACTORIES reg_fun = (REGISTER_ENTITY_INFO_FACTORIES)fun;
				reg_fun(&CoreEntity::EntityInfoFactoryRegistry::get_instance());
			}
			fun = lib_loader.get(lib_handle, "REGISTER_ENTITY_FACTORIES");
			if (fun) {
				REGISTER_ENTITY_FACTORIES reg_fun = (REGISTER_ENTITY_FACTORIES)fun;
				reg_fun(&CoreEntity::EntityFactoryRegistry::get_instance());
			}
		}
	}
}

void ResourceLoader::load_textures(std::unordered_map<std::string, uint32_t>& texture_layers) {
	//SHOULD REMAKE THIS TO ALLOCATE APPROPRIATE TEXTURE ARRAY DEPTH
	textures_array = std::make_unique<Texture3D>();
	textures_array->setup_texture_array(512, 512, 5, false);

	auto _load_textures = [&](std::filesystem::path& textures_path, std::string& source_name) {
		for (const auto& entry : std::filesystem::directory_iterator(textures_path)) {
			if (!entry.is_regular_file()) continue;
			std::filesystem::path file_path = entry.path();
			if (file_path.extension() == ".png") {
				std::string texture_UID = "Texture:" + source_name + ":" + file_path.filename().string();
				texture_layers.emplace(texture_UID, textures_array->get_current_size());
				bool success = textures_array->add_2D_texture(file_path.string().c_str());
				if (!success)
					throw std::runtime_error(std::string("Can't load texture ") + file_path.filename().string());
			}
		}
	};

	std::filesystem::path textures_path = resources_root / "Resources/textures";
	std::string source_name = "Core";
	_load_textures(textures_path, source_name);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		textures_path = mod_info.mod_folder_path / "Resources/textures";
		if (!std::filesystem::exists(textures_path)) continue;
		source_name = mod_info.mod_folder_path.filename().string();
		_load_textures(textures_path, source_name);
	}

	textures_array->bind(0);
}

void ResourceLoader::load_sprites(std::unordered_map<std::string, uint32_t>& texture_layers) const {
	//add empty sprite with global_ID = 0
	CoreResource::SpriteManager::get_instance().add_sprite("Sprite:Core:Empty", 0, 0, 0, 0, 0.0f, 0);

	auto _load_sprites = [&](std::filesystem::path& sprites_path) {
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
					auto it = texture_layers.find(texture_UID);
					if (it == texture_layers.end())
						ResourceLoading::throw_reference_error(UID, texture_UID);
					uint32_t texture_array_id = it->second;

					auto& sprite = s["sprite"];
					uint32_t u0 = sprite["U0"].get_as<uint32_t>();
					uint32_t v0 = sprite["V0"].get_as<uint32_t>();
					uint32_t w = sprite["w"].get_as<uint32_t>();
					uint32_t h = sprite["h"].get_as<uint32_t>();
					float base_size = sprite["base_size"].get_as<float>();

					CoreResource::SpriteManager::get_instance().add_sprite(UID, u0, v0, w, h, base_size, texture_array_id);
				}
				catch (const std::exception& e) {
					ResourceLoading::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
				}
			}
		}
	};
	
	std::filesystem::path sprites_path = resources_root / "Resources/data/sprites_data";
	_load_sprites(sprites_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		sprites_path = mod_info.mod_folder_path / "Resources/data/sprites_data";
		if (!std::filesystem::exists(sprites_path)) continue;
		_load_sprites(sprites_path);
	}
}

void ResourceLoader::load_lights() const {
	//add empty light with global_ID = 0
	CoreResource::LightsManager::get_instance().add_light("Light:Core:Empty", 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	auto _load_lights = [&](std::filesystem::path& lights_path) {
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
					CoreResource::LightsManager::get_instance().add_light(UID, radius, glm::vec3(r, g, b));
				}
				catch (const std::exception& e) {
					ResourceLoading::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
				}
			}
		}
	};

	std::filesystem::path lights_path = resources_root / "Resources/data/lights_data";
	_load_lights(lights_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		lights_path = mod_info.mod_folder_path / "Resources/data/lights_data";
		if (!std::filesystem::exists(lights_path)) continue;
		_load_lights(lights_path);
	}
}

void ResourceLoader::load_particles() {
	//add empty particle with global_ID = 0

}

void ResourceLoader::load_effects() const {
	//add empty effect with global_ID = 0
	CoreResource::EffectsManager::get_instance().add_effect_info(CoreResource::EffectType::isBuff, "Effect:Core:Empty", CoreResource::EffectStatType::isTypeless, 0.0f, 0);

	auto _load_effects = [&](std::filesystem::path& effects_path) {
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
					auto sprite_global_id = CoreResource::SpriteManager::get_instance().get_sprite_id(sprite_UID);
					if (!sprite_global_id)
						ResourceLoading::throw_reference_error(UID, sprite_UID);
					uint8_t effect_type_id = effect["effect_type"].get_as<uint8_t>();
					uint8_t stat_type_id = effect["stat_type"].get_as<uint8_t>();
					float value = effect["value"].get_as<float>();

					uint32_t particle_id = 0;
					float p_spawn_cd = 0.0f, dmg_cd = 0.0f;
					if (effect.has_child("dmg_cd")) dmg_cd = effect["dmg_cd"].get_as<float>();
					if (effect.has_child("particle")) {
						auto& p = effect["particle"];

					}
					CoreResource::EffectsManager::get_instance().add_effect_info(
						(CoreResource::EffectType)effect_type_id, UID, (CoreResource::EffectStatType)stat_type_id, value,
						sprite_global_id.value(), particle_id, p_spawn_cd, dmg_cd
					);
				}
				catch (const std::exception& e) {
					ResourceLoading::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
				}
			}
		}
	};

	std::filesystem::path effects_path = resources_root / "Resources/data/effects_data";
	_load_effects(effects_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		effects_path = mod_info.mod_folder_path / "Resources/data/effects_data";
		if (!std::filesystem::exists(effects_path)) continue;
		_load_effects(effects_path);
	}
}

void ResourceLoader::load_items_data(std::vector<DataParser>& item_data_parsers) {
	//add "Air" info with global_ID = 0
	CoreObject::ObjectManager::get_instance().create_object_info<CoreObject::ObjectInfo, CoreObject::ObjectType::None>("Item:Core:Air");
	//add "MultiBlockTile" info with global_ID = 1
	CoreObject::ObjectManager::get_instance().create_object_info<CoreObject::ObjectInfo, CoreObject::ObjectType::isMultiBlockTile>("Item:Core:MultiBlockTile");

	auto _load_items = [&](std::filesystem::path& items_path) {
		for (const auto& entry : std::filesystem::directory_iterator(items_path)) {
			if (!entry.is_regular_file()) continue;
			std::filesystem::path file_path = entry.path();
			if (file_path.extension() != data_extension) continue;

			DataParser& parser = item_data_parsers.emplace_back();
			parser.parse_data(file_path, DEBUG);
			for (const auto& item : parser.node_root.array) {
				if (!item.has_child("class_UID"))
					throw std::runtime_error("Item entry in " + file_path.string() + " has no class UID!");
				std::string class_uid = item["class_UID"].get_as<std::string>();
				auto factory_id = CoreObject::ObjectFactoryRegistry::get_instance().get_factory_id(class_uid);
				if (!factory_id)
					throw std::runtime_error("Info Class " + class_uid + " from file " + file_path.string() + " not found!");
				CoreObject::_ObjectFactoryI* factory = CoreObject::ObjectFactoryRegistry::get_instance().get_factory(factory_id.value());
				factory->add_object(item);
			}
		}
	};
	
	std::filesystem::path items_path = resources_root / "Resources/data/items_data";
	_load_items(items_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		items_path = mod_info.mod_folder_path / "Resources/data/items_data";
		if (!std::filesystem::exists(items_path)) continue;
		_load_items(items_path);
	}
}

void ResourceLoader::load_crafts() const {
	auto _load_crafts = [&](std::filesystem::path& crafts_path) {
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
					auto item_info_id = CoreObject::ObjectManager::get_instance().get_object_id(result_UID);
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
						auto item_info_ID = CoreObject::ObjectManager::get_instance().get_object_id(item_UID);
						if (!item_info_ID.has_value())
							ResourceLoading::throw_reference_error(result_UID, item_UID);
						uint16_t item_ID = item_info_ID.value();
						uint16_t item_amount = i["item_amount"].get_as<uint16_t>();
						craft_pairs.emplace_back((uint16_t)item_ID, item_amount);
					}
					CraftingSystem::get_instance().add(item_id, amount, CraftCondition(condition_id), std::move(craft_pairs));
				}
				catch (const std::exception& e) {
					ResourceLoading::throw_resource_error(result_UID, std::string("In file ") + file_path.string() + ": " + e.what());
				}
			}
		}
	};

	std::filesystem::path crafts_path = resources_root / "Resources/data/crafts_data";
	_load_crafts(crafts_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		crafts_path = mod_info.mod_folder_path / "Resources/data/crafts_data";
		if (!std::filesystem::exists(crafts_path)) continue;
		_load_crafts(crafts_path);
	}
}

void ResourceLoader::load_animation_clips_data() const {
	auto _load_clips = [&](std::filesystem::path& clips_path) {
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
					CoreAnimation::AnimationClip& clip = CoreAnimation::AnimationClipManager::get_instance().add_clip(UID);

					bool loop = clip_data["loop"].get_as<bool>();
					float frame_time = clip_data["frame_time"].get_as<float>();
					auto& sprites = clip_data["sprites"].array;

					clip.looping = loop;
					uint32_t sprites_size = sprites.size();
					clip.sprites.reserve(sprites_size);
					clip.set_frame_time(frame_time);
					for (uint32_t i = 0; i < sprites_size; i++) {
						std::string sprite_UID = sprites[i].get_as<std::string>();
						auto sprite_global_id = CoreResource::SpriteManager::get_instance().get_sprite_id(sprite_UID);
						if (!sprite_global_id.has_value())
							ResourceLoading::throw_reference_error(UID, sprite_UID);
						else clip.sprites.emplace_back(sprite_global_id.value());
					}
				}
				catch (const std::exception& e) {
					ResourceLoading::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
				}
			}
		}
	};

	std::filesystem::path clips_path = resources_root / "Resources/data/anim_data/clips";
	_load_clips(clips_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		clips_path = mod_info.mod_folder_path / "Resources/data/anim_data/clips";
		if (!std::filesystem::exists(clips_path)) continue;
		_load_clips(clips_path);
	}
}

void ResourceLoader::load_animation_animators_data() const {
	auto _load_animators = [&](std::filesystem::path& animators_path) {
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
					CoreAnimation::Animator& animator = CoreAnimation::AnimatorManager::get_instance().add_animator(UID);
					auto& clips = animator_data["clips"].array;
					uint32_t clips_size = clips.size();
					animator.clips.reserve(clips_size);
					for (uint32_t i = 0; i < clips_size; i++) {
						std::string clip_UID = clips[i].get_as<std::string>();
						auto clip_id_opt = CoreAnimation::AnimationClipManager::get_instance().get_clip_id(clip_UID);
						if (!clip_id_opt.has_value())
							ResourceLoading::throw_reference_error(UID, clip_UID);
						else animator.clips.emplace_back(clip_id_opt.value());
					}
				}
				catch (const std::exception& e) {
					ResourceLoading::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
				}
			}
		}
	};

	std::filesystem::path animators_path = resources_root / "Resources/data/anim_data/animators";
	_load_animators(animators_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		animators_path = mod_info.mod_folder_path / "Resources/data/anim_data/animators";
		if (!std::filesystem::exists(animators_path)) continue;
		_load_animators(animators_path);
	}
}

void ResourceLoader::load_entities_data(std::vector<DataParser>& entity_data_parsers) const {
	auto _load_entities = [&](std::filesystem::path& entities_path) {
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
				auto factory_id = CoreEntity::EntityInfoFactoryRegistry::get_instance().get_factory_id(info_class_uid);
				if (!factory_id.has_value())
					throw std::runtime_error("Info Class " + info_class_uid + " from file " + file_path.string() + " not found!");
				CoreEntity::EntityInfoFactoryI* factory = CoreEntity::EntityInfoFactoryRegistry::get_instance().get_factory(factory_id.value());
				factory->add_entity_info(entity_data);
			}
		}
	};
	
	std::filesystem::path entities_path = resources_root / "Resources/data/entity_data";
	_load_entities(entities_path);
	for (const auto& mod_info : ModsManager::get_instance().get_mods()) {
		entities_path = mod_info.mod_folder_path / "Resources/data/entity_data";
		if (!std::filesystem::exists(entities_path)) continue;
		_load_entities(entities_path);
	}
}

void ResourceLoader::resolve_items_dependencies(const std::vector<DataParser>& item_data_parsers) const {
	uint16_t object_ID = 2;
	for (const auto& parser : item_data_parsers) {
		for (const auto& data_node : parser.node_root.array) {
			CoreObject::ObjectManager::get_instance().get_object_info(object_ID)->fill_dependent_data(data_node);
			object_ID++;
		}
	}
}

void ResourceLoader::resolve_entities_dependencies(const std::vector<DataParser>& entity_data_parsers) const {
	uint16_t entity_ID = 0;
	for (const auto& parser : entity_data_parsers) {
		for (const auto& data_node : parser.node_root.array) {
			CoreEntity::EntityInfoManager::get_instance().get_entity_info(entity_ID)->fill_dependent_data(data_node);
			entity_ID++;
		}
	}
}