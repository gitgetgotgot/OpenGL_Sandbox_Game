#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <Physics/Colliders.h>
#include <Utility/DataParser.h>
#include <Utility/DynamicArray.h>
#include <iostream>

constexpr uint32_t CHUNK_SIZE = 64;

struct WorldSlot {
	uint16_t tile_id = 0;
	uint16_t wall_id = 0;
	uint16_t flags = 0;
};

struct InventorySlot {
	uint16_t item_id = 0;
	uint16_t amount = 0;
};

struct Universal_UBO_Data {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::ivec2 world_size;
	float day_ratio;
	float padding;
};

struct SpriteData {
	glm::mat4 modelMatrix;
	glm::vec2 tex_coords[4];
	float light_level = 1.f, opacity = 1.f; //light level and opacity are from 0.0 to 1.0 (can be used for weather system)
	float padding[2]{};
};

struct ShaderLightingInfo {
	ShaderLightingInfo() {}
	ShaderLightingInfo(uint8_t r, uint8_t g, uint8_t b, uint8_t global_value, uint8_t source_value) {
		color.x = r; color.y = g; color.z = b;
		global_light_value = global_value;
		source_light_value = source_value;
	}
	glm::u8vec3 color;
	uint8_t global_light_value;
	uint8_t source_light_value;
};

struct EffectEntry {
	uint16_t id = 0;
	float duration = 0.0f;
};

constexpr float TEXT_DAMAGE_LIFETIME = 1.0f;
struct DamageText {
	DamageText(glm::vec2 start_pos, glm::vec4 color, std::string damage, float text_max_height) :
		start_pos{ start_pos }, color{ color }, text{ damage }, max_height{ text_max_height } {
		text_height = max_height * 0.2f;
	}
	//return true if should be deleted
	bool updateText(float deltaTime, float block_size) {
		lifeTime += deltaTime;
		start_pos.y += block_size * deltaTime;
		if (lifeTime < TEXT_DAMAGE_LIFETIME * 0.5)
			text_height = lifeTime / (TEXT_DAMAGE_LIFETIME * 0.5) * (0.8 * max_height);
		if (lifeTime >= TEXT_DAMAGE_LIFETIME) {
			return true;
		}
		else return false;
	}
	glm::vec4 color;
	glm::vec2 start_pos;
	float text_height;
	float max_height;
	float lifeTime = 0.f;
	std::string text;
};

enum ObjectType : uint8_t {
	None, isMultiBlockTile, isWall, isBlock, isMultiBlock, isItem, isWeapon, isAmmo, isConsumable, isArmor, isArtifact, isCoin
};

class ObjectInfo {
public:
	ObjectInfo(ObjectType type, std::string_view uid) :
		objectType{ type }, uid{ uid } {
		size_t pos = uid.rfind(':');
		if (pos == std::string_view::npos) name = uid; //if ':' not found, but this shouldn't happen if resource file is correct
		else name = uid.substr(pos + 1);
	}
	virtual ~ObjectInfo() {}

	ObjectType objectType = ObjectType::None;
	std::string_view uid, name;
	DynamicArray<uint32_t> sprites;
	DynamicArray<uint16_t> drops;
	DynamicArray<EffectEntry> effects;
	DynamicArray<uint16_t> lights;
};

class ObjectsDB {
public:
	inline static std::unordered_set<std::string/*UID*/> UID_set;
	inline static std::unordered_map<std::string_view/*UID*/, uint32_t/*global_ID*/> UID_to_ID;
	inline static std::vector<std::unique_ptr<ObjectInfo>> objectInfo;
};

class ObjectFactoryI {
public:
	virtual void add_object(DataNode& data) = 0;
	virtual ~ObjectFactoryI() = default;
};

namespace GameObjects {
	//base method that fills base data (sprites, effects, lights), except dependencies (drops)
	void fill_base_data(DataNode& data, ObjectInfo* info);
	void throw_reference_error(std::string_view item_uid, std::string_view resource_uid);
	void throw_resource_error(std::string_view item_uid, std::string_view msg);
}

template<typename T, ObjectType main_type = ObjectType::None>
class ObjectInfoFactory : public ObjectFactoryI {
public:
	static_assert(std::is_base_of_v<ObjectInfo, T>, "T must inherit from ObjectInfo");

	//virtual main method to add info
	void add_object(DataNode& data) override {
		create_object_info(data);
	}
protected:
	//base method that adds info to ObjectsDB
	ObjectInfo* create_object_info(DataNode& data) {
		std::string UID = data["UID"].get_as<std::string>();
		auto set_it = ObjectsDB::UID_set.emplace(UID);
		std::string_view uid_view = *(set_it.first);
		ObjectsDB::UID_to_ID.emplace(uid_view, ObjectsDB::objectInfo.size());

		ObjectInfo* info = ObjectsDB::objectInfo.emplace_back(std::make_unique<T>(main_type, uid_view)).get();
		GameObjects::fill_base_data(data, info);
		return info;
	}
};

class ObjectFactoryRegistry {
public:
	static ObjectFactoryRegistry* get_instance() {
		static ObjectFactoryRegistry registry;
		return &registry;
	}
	void register_factory(std::string class_uid, std::unique_ptr<ObjectFactoryI> factory) {
		object_class_to_factory_id.emplace(class_uid, factories.size());
		factories.emplace_back(std::move(factory));
	}
	ObjectFactoryI* get_factory(uint32_t factory_id) {
		return factories[factory_id].get();
	}
	std::optional<uint32_t> get_factory_id(std::string class_uid) {
		auto it = object_class_to_factory_id.find(class_uid);
		if (it == object_class_to_factory_id.end()) return std::nullopt;
		else return it->second;
	}
private:
	std::unordered_map<std::string, uint32_t> object_class_to_factory_id;
	std::vector<std::unique_ptr<ObjectFactoryI>> factories;
	ObjectFactoryRegistry() {}
	~ObjectFactoryRegistry() {}
};

//OBJECT COMPONENTS
class ObjectComponent {
public:
	virtual ~ObjectComponent() {}
};

class MultiBlockTileComponent : public ObjectComponent {
public:
	MultiBlockTileComponent(uint16_t column, uint16_t line) :column{ column }, line{ line } {}
	uint16_t column, line;
};

class ChestComponent : public ObjectComponent {
public:
	InventorySlot chest_slots[40];
};

class DoorComponent : public ObjectComponent {
public:
	DoorComponent(uint8_t state) : door_state{ state } {}
	uint8_t door_state = 0; //0 - closed, 1 - opened to left, 2 - opened to right
};

struct EntityInfoText {
	bool isActive = false;
	std::string info;
	glm::vec2 start_pos = {0.f, 0.f};
};

struct ActiveWeapon { //draw sprite for active weapon (also should be used for tools, not only weapons, but it's not important for now)
	bool isActive = false;
	bool hitboxIsActive = false;
	bool points_to_left;
	bool render_upside_down = false;
	int weapon_id;
	float angle = 0.f;
	float sizeX, sizeY;
	float time_spent = 0.f;
	float speed_factor = 1.f;
	float power = 1.f; //for pickaxe, axe, etc.
	float projectile_attack_cd = 0.f; //cooldown of attack(shooting arrow from bow, etc.)
	float projectile_attack_current_cd = 0.f;
	float hit_cd = 0.f; //cd for each enemy that was hit by swinging weapon
	float time_to_finish_swing = 0.f;
	glm::mat4 modelMatrix;
	glm::vec2 tex_coords[4];
	float tex_id = 0.f;
	//WeaponType weapon_type;
	Collider_2D_OBB hitbox;
};

//struct that is used for breaking different objects
struct ActiveBreakableObject {
	void start_breaking_object(int column, int line, float time_to_break) {
		isBreaking = true;
		this->column = column;
		this->line = line;
		time_breaking = 0.f;
		this->time_to_break = time_to_break;
	}
	bool update_breaking_object(int column, int line, float deltaTime) {
		if (column != this->column || line != this->line) {
			start_breaking_object(column, line, this->time_to_break);
			return false;
		}
		time_breaking += deltaTime;
		if (time_breaking >= time_to_break) {
			isBreaking = false;
			return true;
		}
		return false;
	}
	bool isBreaking = false;
	int column, line;
	float time_to_break = 1.0f; //default
	float time_breaking = 0.5f;
};
