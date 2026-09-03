#pragma once

#include <Audio/AudioManager.h>
#include <Utility/CommonData.h>
#include <Utility/Effects.h>
#include <Utility/DynamicArray.h>
#include <Utility/Animation.h>
#include <Utility/DataParser.h>
#include <Physics/Colliders.h>

namespace CoreEntity {
	enum EntityMainType : uint8_t {
		isMob, isProjectile, isPet, isNPC
	};
	enum MovementType : uint8_t { //probably should be removed
		isWalking = 0, isSwimming = 1, isFlying = 2
	};

	//structure that contains information about drop from entity
	struct DropInfo {
		uint32_t item_id = 0;
		float chance = 0.f;
		//amount of this item in range [amount1, amount2]
		uint32_t amount1 = 0;
		uint32_t amount2 = 0;
		DropInfo(uint32_t id, float chance, uint32_t amount) : item_id{ id }, chance{ chance }, amount1{ amount }, amount2{ amount } {}
		DropInfo(uint32_t id, float chance, uint32_t amount1, uint32_t amount2) : item_id{ id }, chance{ chance }, amount1{ amount1 }, amount2{ amount2 } {}
	};

	struct DroppedItem {
		bool has_pick_cd = false; //if player is throwing something, then it has cd for 1 second for picking it up
		float cd_time = 0.f;
		int id = 0;
		int amount = 0;
		Collider_2D_AABB hitbox;
		//physics
		bool has_bottom_collision = false;
		float fallingDistance = 0.f;
		float time_in_free_falling = 0.f;
		float Xinc = 0.f;
	};

	class Transform {
	public:
		glm::vec2 pos{ 0.0f };
		glm::vec2 scale{ 1.0f };
		float rotation_angle = 0.0f;
	};

	class EntityInfo {
	public:
		EntityInfo(EntityMainType type, std::string_view UID);
		~EntityInfo() {}
		virtual void fill_data(const DataNode& data);
	public:
		EntityMainType main_type = EntityMainType::isMob;
		uint16_t subtype_ID = 0;
		std::string_view uid, name;
		uint16_t factory_ID = 0;
		DynamicArray<EffectEntry> effects;
		DynamicArray<uint16_t> lights;
	};

	class EntityInfoManager {
	public:
		static EntityInfoManager* get_instance() {
			static EntityInfoManager mgr;
			return &mgr;
		}
		template<typename ClassInfoT, EntityMainType MAIN_TYPE>
		EntityInfo* add_entity_info(const DataNode& data) {
			std::string UID = data["UID"].get_as<std::string>();
			std::string_view uid_view = UID_storage.emplace_back(UID);
			UID_to_ID.emplace(uid_view, entities.size());
			EntityInfo* info = entities.emplace_back(std::make_unique<ClassInfoT>(MAIN_TYPE, uid_view)).get();
			fill_base_data(data, info);
			return info;
		}
		void ClearData();
		EntityInfo* get_entity_info(uint16_t ID);
		std::optional<uint16_t> get_entity_id(std::string_view UID);
		void fill_base_data(const DataNode& data, EntityInfo* info);
	private:
		std::deque<std::string> UID_storage;
		std::unordered_map<std::string_view, uint16_t> UID_to_ID;
		std::vector<std::unique_ptr<EntityInfo>> entities;
	};

	class MobPhysics {
	public:
		inline static float MAX_FALL_SPEED = 20.0f;
		inline static float GRAVITY = 9.8f;

		glm::vec2 linear_velocity{ 0.0f };
		bool affected_by_gravity = true;
		float gravity_scale = 1.0f;
		uint8_t collision = CollisionType::NONE;
		bool platform_collision = false;

		float fallingDistance = 0.f; //to calculate the increment for moving on Y axis
		float time_falling = 0.f;
		int current_Y_max_level; //Y level of current block layer, where the entity is standing or stopped going up from jump
	};

	struct MobStats {
		std::vector<CoreResource::Effect> effects;
		bool applyEffect(CoreResource::Effect& effect) {}

		void inflictDamage(float dmg) {
			HP -= dmg;
		}

		void removeEffect(CoreResource::Effect& effect) {}

		int HP = 100;
		int DEF = 0;
		glm::vec2 speed{ 0.0f };
		float speedFactor = 1.f;
	};

	class EntityBase {
	public:
		EntityBase(uint32_t entity_id) : entity_id{ entity_id } {}
		~EntityBase() {}
		virtual void on_create() = 0;
		virtual void on_destroy() = 0;
		virtual void update() = 0;

		uint32_t entity_id = 0;
		bool flip_x = false;
		Transform transform;
		CoreResource::Sprite* current_sprite;
	};
}
