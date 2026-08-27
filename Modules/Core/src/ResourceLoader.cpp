#include <Core/ResourceLoader.h>
#include <Entities/EntityRegistry.h>
#include <Objects/ObjectRegistry.h>
#include <Objects/GameObjects.h>
#include <Objects/Crafting.h>
#include <Objects/ObjectTypes/WallInfo.h>
#include <Entities/GameEntities.h>
#include <Utility/AppliableComps.h>

void ResourceLoader::Load_Resources() {
	if (DEBUG) {
		data_files_path = "Resources/game_data";
		data_extension = ".json";
	}
	else {
		data_files_path = "Resources/bin";
		data_extension = ".DATA";
	}
	resources_root = RESOURCE_ROOT;

	std::unordered_map<std::string, uint32_t> texture_layers;

	register_core_object_factories();
	register_core_entity_factories();

	// load sequence: textures -> sprites -> lights -> particles -> effects -> items -> crafts -> animations -> entities -> other...
	// resolve dependencies: items, entities

	load_textures(texture_layers);
	load_sprites(texture_layers);
	load_lights();
	load_particles();
	load_effects();
	load_items_data();
	load_crafts();
	//load_animations_data();
	//load_entities_data();

	resolve_items_dependencies();
	//resolve_entities_dependencies();
}

void ResourceLoader::load_textures(std::unordered_map<std::string, uint32_t>& texture_layers) {
	std::filesystem::path core_textures_path = resources_root / "Resources/textures";

	textures_array = std::make_unique<Texture3D>();
	textures_array->setup_texture_array(512, 512, 5, false);

	for (auto& entry : std::filesystem::directory_iterator(core_textures_path)) {
		if (!entry.is_regular_file()) continue;

		std::filesystem::path file_path = entry.path();
		if (file_path.extension() == ".png") {
			std::string texture_UID = "Texture:Core:" + file_path.filename().string();
			texture_layers.emplace(texture_UID, textures_array->get_current_size());
			bool success = textures_array->add_2D_texture(file_path.string().c_str());
			if (!success)
				throw std::runtime_error(std::string("Can't load texture ") + file_path.filename().string());
		}
	}
	textures_array->bind(0);
}

void ResourceLoader::load_sprites(std::unordered_map<std::string, uint32_t>& texture_layers) const {
	SpriteManager* sprite_mgr = SpriteManager::get_instance();
	
	//add empty sprite with global_ID = 0
	sprite_mgr->add_sprite("Sprite:Core:Empty", 0, 0, 0, 0, 0.0f, 0);

	std::filesystem::path core_sprites_path = resources_root / data_files_path / "sprites_data";

	for (auto& entry : std::filesystem::directory_iterator(core_sprites_path)) {
		if (!entry.is_regular_file()) continue;

		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);

		for (const auto& s : parser.node_root.array) {
			if(!s.has_child("UID"))
				throw std::runtime_error("Sprite entry in " + file_path.string() + " has no UID!");
			std::string UID = s["UID"].get_as<std::string>();
			try {
				std::string texture_UID = s["texture"].get_as<std::string>();
				uint32_t padding = s["padding"].get_as<uint32_t>();
				auto it = texture_layers.find(texture_UID);
				if (it == texture_layers.end())
					GameObjects::throw_reference_error(UID, texture_UID);
				uint32_t texture_array_id = it->second;

				auto& sprite = s["sprite"];
				uint32_t u0 = sprite["U0"].get_as<uint32_t>();
				uint32_t v0 = sprite["V0"].get_as<uint32_t>();
				uint32_t w = sprite["w"].get_as<uint32_t>();
				uint32_t h = sprite["h"].get_as<uint32_t>();
				float base_size = sprite["base_size"].get_as<float>();

				sprite_mgr->add_sprite(UID, u0, v0, w, h, base_size, texture_array_id);
			}
			catch (const std::exception& e) {
				GameObjects::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::load_lights() const {
	ComponentsManager* comps_mgr = ComponentsManager::get_instance();

	//add empty light with global_ID = 0
	comps_mgr->add_light("Light:Core:Empty", 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	std::filesystem::path core_lights_path = resources_root / data_files_path / "lights_data";

	for (auto& entry : std::filesystem::directory_iterator(core_lights_path)) {
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
				comps_mgr->add_light(UID, radius, glm::vec3(r, g, b));
			}
			catch (const std::exception& e) {
				GameObjects::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::load_particles() {
	//add empty particle with global_ID = 0

}

void ResourceLoader::load_effects() const {
	Effects::EffectsManager* effects_mgr = Effects::EffectsManager::get_instance();
	SpriteManager* sprite_mgr = SpriteManager::get_instance();

	//add empty effect with global_ID = 0
	effects_mgr->add_effect_info(Effects::EffectType::isBuff, "Effect:Core:Empty", Effects::StatType::isTypeless, 0.0f, 0);

	std::filesystem::path core_effects_path = resources_root / data_files_path / "effects_data";

	for (auto& entry : std::filesystem::directory_iterator(core_effects_path)) {
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
				auto sprite_global_id = sprite_mgr->get_sprite_id(sprite_UID);
				if (!sprite_global_id)
					GameObjects::throw_reference_error(UID, sprite_UID);
				uint8_t effect_type_id = effect["effect_type"].get_as<uint8_t>();
				uint8_t stat_type_id = effect["stat_type"].get_as<uint8_t>();
				float value = effect["value"].get_as<float>();

				uint32_t particle_id = 0;
				float p_spawn_cd = 0.0f, dmg_cd = 0.0f;
				if (effect.has_child("dmg_cd")) dmg_cd = effect["dmg_cd"].get_as<float>();
				if (effect.has_child("particle")) {
					auto& p = effect["particle"];

				}
				effects_mgr->add_effect_info(
					(Effects::EffectType)effect_type_id, UID, (Effects::StatType)stat_type_id, value,
					sprite_global_id.value(), particle_id, p_spawn_cd, dmg_cd
				);
			}
			catch (const std::exception& e) {
				GameObjects::throw_resource_error(UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::load_items_data() const {
	ComponentsManager* comps_mgr = ComponentsManager::get_instance();
	Effects::EffectsManager* effects_mgr = Effects::EffectsManager::get_instance();
	SpriteManager* sprite_mgr = SpriteManager::get_instance();
	ObjectFactoryRegistry* registry = ObjectFactoryRegistry::get_instance();

	//add "Air" info with global_ID = 0
	auto uid0 = ObjectsDB::UID_set.emplace("Item:Core:Air").first;
	std::string_view uid_view0 = *uid0;
	ObjectsDB::UID_to_ID.emplace(uid_view0, ObjectsDB::objectInfo.size());
	ObjectsDB::objectInfo.emplace_back(std::make_unique<ObjectInfo>(ObjectType::None, uid_view0));

	//add "MultiBlockTile" info with global_ID = 1
	uid0 = ObjectsDB::UID_set.emplace("Item:Core:MultiBlockTile").first;
	uid_view0 = *uid0;
	ObjectsDB::UID_to_ID.emplace(uid_view0, ObjectsDB::objectInfo.size());
	ObjectsDB::objectInfo.emplace_back(std::make_unique<ObjectInfo>(ObjectType::isMultiBlockTile, uid_view0));

	std::filesystem::path core_items_path = resources_root / data_files_path / "items_data";

	for (auto& entry : std::filesystem::directory_iterator(core_items_path)) {
		if (!entry.is_regular_file()) continue;

		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);

		for (auto& item : parser.node_root.array) {
			if (!item.has_child("class_UID"))
				throw std::runtime_error("Item entry in " + file_path.string() + " has no class UID!");
			std::string class_uid = item["class_UID"].get_as<std::string>();
			auto factory_id = registry->get_factory_id(class_uid);
			if (!factory_id)
				throw std::runtime_error("Info Class " + class_uid + " from file " + file_path.string() + " not found!");
			ObjectFactoryI* factory = registry->get_factory(factory_id.value());
			factory->add_object(item);
		}
	}
}

void ResourceLoader::load_crafts() const {
	CraftingSystem* craft_sys = CraftingSystem::get_instance();

	std::filesystem::path core_crafts_path = resources_root / data_files_path / "crafts_data";

	for (auto& entry : std::filesystem::directory_iterator(core_crafts_path)) {
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
				auto result_it = ObjectsDB::UID_to_ID.find(result_UID);
				if (result_it == ObjectsDB::UID_to_ID.end())
					throw std::runtime_error("Craft entry in " + file_path.string() + ": <" + result_UID + "> doesn't exist!");
				uint32_t item_id = result_it->second;
				uint16_t amount = craft["amount"].get_as<uint16_t>();
				uint16_t condition_id = craft["condition"].get_as<uint16_t>();

				auto& items = craft["items"].array;
				std::vector<CraftingPair> craft_pairs;
				craft_pairs.reserve(items.size());

				for (auto& i : items) {
					std::string item_UID = i["item_UID"].get_as<std::string>();
					auto item_it = ObjectsDB::UID_to_ID.find(item_UID);
					if (item_it == ObjectsDB::UID_to_ID.end())
						GameObjects::throw_reference_error(result_UID, item_UID);
					uint32_t item_ID = item_it->second;
					uint16_t item_amount = i["item_amount"].get_as<uint16_t>();
					craft_pairs.emplace_back((uint16_t)item_ID, item_amount);
				}
				craft_sys->add(item_id, amount, CraftCondition(condition_id), std::move(craft_pairs));
			}
			catch (const std::exception& e) {
				GameObjects::throw_resource_error(result_UID, std::string("In file ") + file_path.string() + ": " + e.what());
			}
		}
	}
}

void ResourceLoader::load_animations_data() {
	std::ifstream file("Resources/animations.json");
	nlohmann::json json;
	file >> json;

	for (auto& anim : json) {
		uint32_t id = anim["id"];
		std::vector<nlohmann::json> anims = anim["animator"].get<std::vector<nlohmann::json>>();
		AnimatorManager::get_instance()->add_animator();
		Animator& animator = *AnimatorManager::get_instance()->get_animator(id);
		animator.clips.reserve(anims.size());

		for (auto& a : anims) {
			uint32_t clip_id = a["clip_id"];
			AnimationClipManager::get_instance()->add_clip();
			AnimationClip& clip = *AnimationClipManager::get_instance()->get_clip(clip_id);
			animator.clips.emplace_back(clip_id);

			bool loop = a["loop"];
			float frame_time = a["frame_time"];
			std::vector<uint32_t> sprites = a["sprites"].get<std::vector<uint32_t>>();

			clip.looping = loop;
			clip.sprites = std::move(sprites);
			clip.set_frame_time(frame_time);
		}
	}
}

void ResourceLoader::load_entities_data() {
	std::string RESOURCE_DIR = RESOURCE_ROOT;
	std::filesystem::path core_entities_path = RESOURCE_DIR + "Resources/game_data/entity_data";

	for (auto& entry : std::filesystem::directory_iterator(core_entities_path)) {
		if (!entry.is_regular_file()) continue;

		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != ".json") continue;

		std::ifstream file(file_path);
		if (!file.is_open()) {
			std::cerr << "[Entity Loader] Cannot open: " << file_path << "\n";
			
		}
		nlohmann::json json;
		file >> json;

		for (auto& entity : json) {
			//basic common info
			std::string entity_UID = entity["UID"].get<std::string>();
			uint32_t type_id = entity["type"].get<uint32_t>();
			uint32_t effect_id = 0, light_id = 0;

			GameEntity::EntityType entity_type = (GameEntity::EntityType)type_id;
			if (entity_type == GameEntity::EntityType::isMob) {
				uint32_t mob_type_id = entity["mob_type"];
				uint32_t move_type_id = entity["move_type"];
				float HP = entity["HP"], DEF = entity["DEF"], DMG = entity["DMG"];
				float speed_x = entity["speed_x"], speed_y = entity["speed_y"];
				float hitbox_w = entity["hitbox_w"], hitbox_h = entity["hitbox_h"];
				std::vector<nlohmann::json> drop = entity["drop"].get<std::vector<nlohmann::json>>();
				std::vector<GameEntity::DropInfo> drops;
				drops.reserve(drop.size());
				for (auto& d : drop) {
					uint32_t drop_id = d["item_id"];
					float chance = d["chance"];
					uint32_t min = d["min"], max = d["max"];
					drops.emplace_back(drop_id, chance, min, max);
				}
				uint32_t animator_id = entity["animator_id"];

				GameEntity::MobType mob_type = (GameEntity::MobType)mob_type_id;
				if (mob_type == GameEntity::MobType::isEnemy) {
					uint32_t enemy_type_id = entity["enemy_type"];
					//GameEntity::EntityDB::entityInfo[id] = std::make_unique<GameEntity::EnemyInfo>(name, (GameEntity::EnemyType)enemy_type_id,
						//(GameEntity::MovementType)move_type_id, HP, DMG, DEF, speed_x, speed_y, glm::vec2(hitbox_w, hitbox_h), std::move(drops), animator_id);
				}
				else {
					//GameEntity::EntityDB::entityInfo[id] = std::make_unique<GameEntity::MobInfo>(name, mob_type, (GameEntity::MovementType)move_type_id,
						//HP, DMG, DEF, speed_x, speed_y, glm::vec2(hitbox_w, hitbox_h), std::move(drops), animator_id);
				}
			}
		}
	}
}

void ResourceLoader::resolve_items_dependencies() const {
	std::filesystem::path core_items_path = resources_root / data_files_path / "items_data";

	for (auto& entry : std::filesystem::directory_iterator(core_items_path)) {
		if (!entry.is_regular_file()) continue;

		std::filesystem::path file_path = entry.path();
		if (file_path.extension() != data_extension) continue;

		DataParser parser;
		parser.parse_data(file_path, DEBUG);

		for (auto& item : parser.node_root.array) {
			std::string UID = item["UID"].get_as<std::string>();
			std::string_view uid_view = UID;
			uint32_t global_ID = ObjectsDB::UID_to_ID[uid_view];
			bool loaded = false;

			if (item.has_child("drop_UID")) {
				
			}
			if (item.has_child("proj_UID")) {
				
			}
			if (item.has_child("entity_UID")) {
				
			}
		}
	}

}

void ResourceLoader::resolve_entities_dependencies() {

}