#pragma once
#include <Utility/CommonData.h>
#include <Utility/DataParser.h>
#include <Utility/DynamicArray.h>

/*constexpr float TEXT_DAMAGE_LIFETIME = 1.0f;
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
};*/

namespace CoreObject {
	enum ObjectType : uint8_t {
		None, isMultiBlockTile, isWall, isBlock, isMultiBlock, isItem, isWeapon, isAmmo, isConsumable, isArmor, isArtifact, isCoin
	};

	class ObjectInfo {
	public:
		ObjectInfo(ObjectType type);
		virtual ~ObjectInfo() {}
		void set_UID(std::string_view uid);
		virtual void fill_data(const DataNode& data);
		virtual void fill_dependent_data(const DataNode& data);
	public:
		ObjectType objectType = ObjectType::None;
		std::string_view uid, name;
		DynamicArray<uint32_t> sprites;
		DynamicArray<DropEntry> drops;
		DynamicArray<EffectEntry> effects;
		DynamicArray<uint16_t> lights;
	};
}

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
	//Collider_2D_OBB hitbox;
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
