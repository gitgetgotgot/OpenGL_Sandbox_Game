#include "Entities/EntitySystem.h"
#include "Entities/EntityClasses/Mob.h"
#include "Entities/EntityInfoManager.h"
#include "Entities/EntityFactory.h"
#include <IOSystem/SystemContext.h>
#include <Utility/TimeManager.h>
#include <Utility/Math.h>
#include <Objects/ObjectTypes/BlockInfo.h>
#include <Objects/ObjectManager.h>

void CoreEntity::EntitySystem::init() {
	entities.reserve(MAX_ENTITIES_RENDER);
	entity_render_buf.reserve(MAX_ENTITIES_RENDER);
	entity_render_buf.resize(MAX_ENTITIES_RENDER);

	entity_sp = std::make_unique<ShaderProgram>("Resources/shaders/sprites_entity.vert", "Resources/shaders/sprites.frag");

	entity_ssbo = std::make_unique<SSBO>();
	entity_ssbo->bind_SSBO(0);
	entity_ssbo->set_data(nullptr, sizeof(EntityRenderData) * MAX_ENTITIES_RENDER, GL_DYNAMIC_DRAW);
	
	GLfloat pos[] = {
		0.f, 0.f,
		0.f, 1.f,
		1.f, 1.f,
		1.f, 0.f
	};
	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	instance_vao = std::make_unique<VAO>();
	instance_vao->bind_VAO();
	instance_vbo = std::make_unique<VBO>(pos, sizeof(pos));
	instance_ebo = std::make_unique<EBO>(indices, sizeof(indices));
	instance_vao->link_Attribute(0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
	instance_vao->unbind_VAO();
	instance_vbo->unbind_VBO();
	instance_ebo->unbind_EBO();
}

void CoreEntity::EntitySystem::update() {
	int size = entities.size();
	for (int i = 0; i < size; i++) {
		EntityBase* entity_base = entities[i].get();
		Transform& tr = entity_base->transform;
		EntityInfo* info = EntityInfoManager::get_instance().get_entity_info(entity_base->entity_id);
		EntityRenderData& render_data = entity_render_buf[i];

		//update physics
		if (info->main_type == EntityMainType::isMob) {
			Mob* entity = static_cast<Mob*>(entity_base);
			EntityPhysics& physx = entity->physics;
			float width = entity->hitbox.size.x;
			float height = entity->hitbox.size.y;
			float x = entity->hitbox.center.x - width * 0.5f;
			float y = entity->hitbox.center.y - height * 0.5f;

			int min_slot_x = (int)x - 1;
			int max_slot_x = (int)(x + width) + 2;
			int min_slot_y = (int)y - 1;
			int max_slot_y = (int)(y + height) + 2;

			if (!(physx.collision_mask & CollisionType::BOTTOM)) {
				physx.time_falling += TimeManager::deltaTime;
				if (physx.affected_by_gravity) {
					physx.linear_velocity.y -= EntityPhysics::GRAVITY * physx.gravity_scale * TimeManager::deltaTime;
					physx.linear_velocity.y = std::max(physx.linear_velocity.y, -EntityPhysics::MAX_FALL_SPEED);
				}
				float dY = physx.linear_velocity.y * TimeManager::deltaTime;
				physx.fallingDistance += std::abs(dY);
				//calculate current Y max level where mob is falling down
				if (physx.linear_velocity.y < 0.0f)
					physx.current_Y_max_level = (int)entity->hitbox.center.y;
			}
			//tr.pos += physx.linear_velocity * TimeManager::deltaTime;
			//entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
			glm::vec2 delta_move = physx.linear_velocity;

			physx.collision_mask = CollisionType::NONE;
			physx.platform_collision = false;

			for (int i = min_slot_x; i < max_slot_x; i++) {
				for (int j = min_slot_y; j < max_slot_y; j++) {
					uint32_t slot_index = j * world_width + i;
					WorldSlot* slot = &world_slots_ptr[slot_index];

					if (slot->tile_id != 0) {
						CoreObject::ObjectInfo* obj_info_ptr = CoreObject::ObjectManager::get_instance().get_object_info(slot->tile_id);

						//get main part or complex object
						if (obj_info_ptr->objectType == CoreObject::ObjectType::isMultiBlockTile) {
							CoreObject::ObjectComponent* comp = obj_comps_ptr->find(slot_index)->second.get();
							uint16_t main_slot_x = static_cast<CoreObject::MultiBlockTileComponent*>(comp)->column;
							uint16_t main_slot_y = static_cast<CoreObject::MultiBlockTileComponent*>(comp)->line;
							slot_index = main_slot_y * world_width + main_slot_x;
							slot = &world_slots_ptr[slot_index];
							obj_info_ptr = CoreObject::ObjectManager::get_instance().get_object_info(slot->tile_id);
						}

						//this shouldn't happen, as world slots logically have only these types, air is skipped at the beginning, but i'll leave it for now
						if (obj_info_ptr->objectType != CoreObject::ObjectType::isBlock &&
							obj_info_ptr->objectType != CoreObject::ObjectType::isMultiBlock) continue;

						if (static_cast<CoreObject::BlockInfo*>(obj_info_ptr)->platform_collision &&
							physx.linear_velocity.y <= 0.0f
						) { //if object has platform-like collision
							if (Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j) & CollisionType::BOTTOM && physx.current_Y_max_level > j) {
								physx.time_falling = 0.f;
								physx.fallingDistance = 0.f;
								physx.linear_velocity.y = 0.0f;
								tr.pos.y = float(j) + 1.0f;
								physx.current_Y_max_level = j + 1;
								physx.platform_collision = true;
							}
						}
						else if (static_cast<CoreObject::BlockInfo*>(obj_info_ptr)->collision) //usual collision
							switch (Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j)) {
							case CollisionType::LEFT:
								if (delta_move.x < 0.0f) delta_move.x = 0.0f;
								physx.collision_mask |= CollisionType::LEFT;
								tr.pos.x = float(i) + 1.0f + entity->hitbox.size.x * 0.5f;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								break;
							case CollisionType::RIGHT:
								if (delta_move.x > 0.0f) delta_move.x = 0.0f;
								physx.collision_mask |= CollisionType::RIGHT;
								tr.pos.x = float(i) - entity->hitbox.size.x * 0.5f;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								break;
							case CollisionType::TOP:
								physx.collision_mask |= CollisionType::TOP;
								physx.time_falling = 0.f;
								physx.fallingDistance = 0.f;
								physx.linear_velocity.y = 0.0f;
								delta_move.y = 0.0f;
								tr.pos.y = float(j) - entity->hitbox.size.y;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								break;
							case CollisionType::BOTTOM:
								if (physx.linear_velocity.y > 0.0f) continue;
								physx.collision_mask |= CollisionType::BOTTOM;
								physx.time_falling = 0.f;
								physx.fallingDistance = 0.f;
								physx.linear_velocity.y = 0.0f;
								delta_move.y = 0.0f;
								tr.pos.y = float(j) + 1.0f;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								physx.current_Y_max_level = j + 1;
								break;
							default:
								break;
							}
					}
				}
			}
			//entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);

			tr.pos += delta_move * TimeManager::deltaTime;
			entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);

			if (GameMath::mouse_intersection(
				glm::vec2(x, y), glm::vec2(width, height), glm::vec2(SystemContext::mouse.world_x_pos, SystemContext::mouse.world_y_pos))) {
				//show text about this mob (Name HP/MAX_HP)
			}
		}

		//update entity
		entity_base->update();

		//update render data
		CoreResource::Sprite& sprite = *entity_base->current_sprite;
		glm::vec2 sprite_adjustment(0.0f, sprite.base_size * sprite.ratio.y * 0.5f * tr.scale.y);
		GameMath::update_2D_TRS(
			render_data.modelMatrix,
			tr.pos + sprite_adjustment,
			tr.scale * sprite.ratio * sprite.base_size,
			tr.rotation_angle
		);
		render_data.UV.x = sprite.U0;
		render_data.UV.y = sprite.V0;
		render_data.UV.z = sprite.W;
		render_data.UV.w = sprite.H;
	}

	entity_ssbo->update_data(entity_render_buf.data(), entities.size() * sizeof(EntityRenderData));
}

void CoreEntity::EntitySystem::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	renderer->renderInstancedData(entity_sp, instance_vao, instance_vbo, instance_ebo, 6U, entities.size());
}

void CoreEntity::EntitySystem::set_world_data(
	WorldSlot* world_slots_ptr, uint32_t world_width, uint32_t world_height,
	std::unordered_map<uint32_t, std::unique_ptr<CoreObject::ObjectComponent>>* obj_comps_ptr
) {
	this->world_slots_ptr = world_slots_ptr;
	this->world_width = world_width;
	this->world_height = world_height;
	this->obj_comps_ptr = obj_comps_ptr;
}

bool CoreEntity::EntitySystem::spawn_entity(uint32_t id, glm::vec2 pos) {
	uint32_t factory_id = EntityInfoManager::get_instance().get_entity_info(id)->factory_ID;
	EntityFactoryI* factory = EntityFactoryRegistry::get_instance().get_factory(factory_id);
	entities.emplace_back(factory->spawn(id, pos.x, pos.y));
	return true;
}

bool CoreEntity::EntitySystem::spawn_projectile() {
	return false;
}