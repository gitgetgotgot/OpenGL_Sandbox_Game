#pragma once

#include <Audio/AudioManager.h>
#include <Entities/Effects.h>
#include <Physics/Colliders.h>
#include <Entities/Animation.h>

namespace GameEntity {
	enum EntityType : uint8_t {
		isMob = 0, isProjectile = 1, isPet = 2
	};
	enum MobType : uint8_t {
		isEnemy = 0, isFriendly = 1, isNPC = 2
	};
	enum EnemyType : uint8_t {
		isSlime = 0, isZombie = 1, isFlyingEye = 2
	};
	enum ProjectileType : uint8_t {
		Arrow = 0, Bullet = 1, Throwable = 2, MagicBall = 3
	};
	enum MovementType : uint8_t {
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
		EntityInfo(EntityType type, std::string name, glm::vec2 hitboxSize) :
			type{ type }, name{ name }, hitboxSize{ hitboxSize } {
		}
		~EntityInfo() {}

		EntityType type = EntityType::isMob;
		std::string name;
		glm::vec2 hitboxSize{ 1.0f };
		//usable components (if ID = 0 then no component is used)
		uint32_t effect_comp_id = 0;
		uint32_t light_id = 0;
	};

	class EntityDB {
	public:
		inline static std::vector<std::unique_ptr<EntityInfo>> entityInfo;
	};

	class MobInfo : public EntityInfo {
	public:
		MobInfo(std::string name, MobType mob_type, MovementType move_type, float HP, float DMG, float DEF, float speed_x,
			float speed_y, glm::vec2 hitboxSize, std::vector<DropInfo>&& drop_items, uint32_t animator_id) :
			EntityInfo(EntityType::isMob, name, hitboxSize), HP{ HP }, DMG{ DMG }, DEF{ DEF }, speed_x{ speed_x }, speed_y{ speed_y },
			mob_type{ mob_type }, move_type{ move_type }, drop_items{ std::move(drop_items) }, animator_id{ animator_id } {
		}

		float HP = 0.0f, DMG = 0.0f, DEF = 0.0f;
		float speed_x = 0.0f, speed_y = 0.0f;
		uint32_t animator_id = 0;
		MobType mob_type = MobType::isEnemy;
		MovementType move_type = MovementType::isWalking;
		std::vector<DropInfo> drop_items;
	};

	class EnemyInfo : public MobInfo {
	public:
		EnemyInfo(std::string name, EnemyType enemy_type, MovementType move_type, float HP, float DMG, float DEF, float speed_x,
			float speed_y, glm::vec2 hitboxSize, std::vector<DropInfo>&& drop_items, uint32_t animator_id) :
			MobInfo(name, MobType::isEnemy, move_type, HP, DMG, DEF, speed_x, speed_y, hitboxSize,
				std::move(drop_items), animator_id), enemy_type{ enemy_type } {
		}

		EnemyType enemy_type = EnemyType::isSlime;
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
		std::vector<Effects::Effect> effects;
		bool applyEffect(Effects::Effect& effect) {}

		void inflictDamage(float dmg) {
			HP -= dmg;
		}

		void removeEffect(Effects::Effect& effect) {}

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
		Collider_2D_AABB hitbox;
		Sprite* current_sprite;
	};

	class Mob : public EntityBase {
	public:
		Mob(uint32_t entity_id, glm::vec2 spawn_center) : EntityBase(entity_id) {
			MobInfo* info = static_cast<MobInfo*>(EntityDB::entityInfo[entity_id].get());
			stats.HP = info->HP;
			stats.DEF = info->DEF;
			stats.speed = glm::vec2(info->speed_x, info->speed_y);

			transform.pos = spawn_center;
			hitbox.size = info->hitboxSize;
			hitbox.center = spawn_center;

			anim_controller.set_animator(info->animator_id);

			physics.current_Y_max_level = hitbox.center.y - hitbox.size.y * 0.5f;
		}

		AnimatorController anim_controller;
		MobStats stats;
		MobPhysics physics;
	};

	class EntityFactory {
	public:
		EntityFactory() {}
		~EntityFactory() {}
		virtual std::unique_ptr<EntityBase> spawn_entity(
			uint32_t entity_global_ID,
			float world_x, float world_y
		) = 0;
	};

	class EntityFactoryRegistry {
	public:
		static EntityFactoryRegistry* get_instance() {
			static EntityFactoryRegistry mgr;
			return &mgr;
		}
		void register_factory(std::string class_name, std::unique_ptr<EntityFactory> factory) {
			entity_class_to_factory_id.emplace(class_name, factories.size());
			factories.emplace_back(std::move(factory));
		}
		EntityFactory* get_factory(uint32_t factory_id) {
			return factories[factory_id].get();
		}
		uint32_t get_factory_id(std::string entity_class) {
			return entity_class_to_factory_id[entity_class];
		}
	private:
		std::unordered_map<std::string/*entity_class*/, uint32_t/*factory_global_id*/> entity_class_to_factory_id;
		std::vector<std::unique_ptr<EntityFactory>> factories;
		EntityFactoryRegistry() {}
		~EntityFactoryRegistry() {}
	};
}
