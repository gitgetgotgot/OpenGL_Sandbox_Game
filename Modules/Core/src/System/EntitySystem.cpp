#include "Core/System/EntitySystem.h"
#include "Entities/EntityClasses/Mob.h"
#include "Entities/EntityInfoManager.h"
#include "Entities/EntityFactory.h"
#include <DeviceSystem/DeviceContext.h>
#include <Utility/TimeManager.h>
#include <Utility/Math.h>
#include <Utility/Logger.h>
#include <Objects/ObjectTypes/BlockInfo.h>
#include <Objects/ObjectManager.h>
#include <Resources/TextureArrayMgr.h>

void Engine::Core::EntitySystem::init() {
	entities.reserve(MAX_ENTITIES_RENDER);
	entity_render_buf.reserve(MAX_ENTITIES_RENDER);
	entity_render_buf.resize(MAX_ENTITIES_RENDER);

	std::error_code ec;
	std::filesystem::path vertex_shader_path = "Resources/shaders/entity_sprites.vert";
	std::filesystem::path fragment_shader_path = "Resources/shaders/world_sprites.frag";
	shader = Engine::Graphics::Renderer::instance->create_ShaderHandle(vertex_shader_path, fragment_shader_path, ec);
	if (ec) {
		Engine::Util::Logger::Log("Failed to create Entity shader: " + ec.message());
		return;
	}
	
	float pos[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	uint32_t indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	instance_vl = Engine::Graphics::Renderer::instance->create_VertexLayout(ec);
	instance_vl->add_attribute(0, Engine::Graphics::VertexAttributeType::FLOAT_VEC2, sizeof(glm::vec2), 0);

	instance_vb = Engine::Graphics::Renderer::instance->create_VertexBuffer(ec);
	instance_vb->set_data(pos, sizeof(pos), Engine::Graphics::VertexBuffer::USAGE_STATIC);

	instance_ib = Engine::Graphics::Renderer::instance->create_IndexBuffer(ec);
	instance_ib->set_data(indices, sizeof(indices), Engine::Graphics::IndexBuffer::USAGE_STATIC);

	pipeline = Engine::Graphics::Renderer::instance->create_Pipeline(ec);
	if (!ec) pipeline->init(
		*shader, *instance_vl,
		Engine::Graphics::Pipeline::PipelinePrimitives::Triangle, Engine::Graphics::Pipeline::PipelineScissor::Dynamic,
		Engine::Graphics::Pipeline::PipelineViewport::Dynamic, Engine::Graphics::Pipeline::PipelineBlendMode::Alpha,
		Engine::Graphics::Pipeline::PipelineDepthMode::ReadAndWrite, Engine::Graphics::Pipeline::PipelineCullMode::Back, ec
	);

	// UB

	// SB
	sb = Engine::Graphics::Renderer::instance->create_StorageBuffer(ec);
	sb->set_data(nullptr, sizeof(EntityRenderData) * MAX_ENTITIES_RENDER, Engine::Graphics::StorageBuffer::USAGE_DYNAMIC);

	//pipeline->bind_storage_buffer();
	//pipeline->bind_uniform_buffer();
	//Engine::Resource::TextureArrayManager::Instance().bind_samplers_to_pipeline(*pipeline);
}

void Engine::Core::EntitySystem::uninit() {
	pipeline.reset();
	shader.reset();
	sb.reset();
	instance_ib.reset();
	instance_vl.reset();
	instance_vb.reset();
}

void Engine::Core::EntitySystem::update() {
	int size = entities.size();
	for (int i = 0; i < size; i++) {
		Engine::Entity::EntityBase* entity_base = entities[i].get();
		Transform& tr = entity_base->transform;
		Engine::Entity::EntityInfo* info = Engine::Entity::EntityInfoManager::Instance().get_entity_info(entity_base->entity_id);
		EntityRenderData& render_data = entity_render_buf[i];

		//update physics
		if (info->main_type == Engine::Entity::EntityMainType::isMob) {
			Engine::Entity::Mob* entity = static_cast<Engine::Entity::Mob*>(entity_base);
			Engine::Physics::EntityPhysics& physx = entity->physics;
			float width = entity->hitbox.size.x;
			float height = entity->hitbox.size.y;
			float x = entity->hitbox.center.x - width * 0.5f;
			float y = entity->hitbox.center.y - height * 0.5f;

			int min_slot_x = (int)x - 1;
			int max_slot_x = (int)(x + width) + 2;
			int min_slot_y = (int)y - 1;
			int max_slot_y = (int)(y + height) + 2;

			if (!(physx.collision_mask & Engine::Physics::CollisionType::BOTTOM)) {
				physx.time_falling += Engine::Util::TimeManager::deltaTime;
				if (physx.affected_by_gravity) {
					physx.linear_velocity.y -= Engine::Physics::EntityPhysics::GRAVITY * physx.gravity_scale * Engine::Util::TimeManager::deltaTime;
					physx.linear_velocity.y = std::max(physx.linear_velocity.y, -Engine::Physics::EntityPhysics::MAX_FALL_SPEED);
				}
				float dY = physx.linear_velocity.y * Engine::Util::TimeManager::deltaTime;
				physx.fallingDistance += std::abs(dY);
				//calculate current Y max level where mob is falling down
				if (physx.linear_velocity.y < 0.0f)
					physx.current_Y_max_level = (int)entity->hitbox.center.y;
			}
			//tr.pos += physx.linear_velocity * TimeManager::deltaTime;
			//entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
			glm::vec2 delta_move = physx.linear_velocity;

			physx.collision_mask = Engine::Physics::CollisionType::NONE;
			physx.platform_collision = false;

			for (int i = min_slot_x; i < max_slot_x; i++) {
				for (int j = min_slot_y; j < max_slot_y; j++) {
					uint32_t slot_index = j * world_width + i;
					WorldSlot* slot = &world_slots_ptr[slot_index];

					if (slot->tile_id != 0) {
						Engine::Object::ObjectInfo* obj_info_ptr = Engine::Object::ObjectManager::Instance().get_object_info(slot->tile_id);

						//get main part or complex object
						if (obj_info_ptr->objectType == Engine::Object::ObjectType::isMultiBlockTile) {
							Engine::Object::ObjectComponent* comp = obj_comps_ptr->find(slot_index)->second.get();
							uint16_t main_slot_x = static_cast<Engine::Object::MultiBlockTileComponent*>(comp)->column;
							uint16_t main_slot_y = static_cast<Engine::Object::MultiBlockTileComponent*>(comp)->line;
							slot_index = main_slot_y * world_width + main_slot_x;
							slot = &world_slots_ptr[slot_index];
							obj_info_ptr = Engine::Object::ObjectManager::Instance().get_object_info(slot->tile_id);
						}

						//this shouldn't happen, as world slots logically have only these types, air is skipped at the beginning, but i'll leave it for now
						if (obj_info_ptr->objectType != Engine::Object::ObjectType::isBlock &&
							obj_info_ptr->objectType != Engine::Object::ObjectType::isMultiBlock) continue;

						if (static_cast<Engine::Object::BlockInfo*>(obj_info_ptr)->platform_collision &&
							physx.linear_velocity.y <= 0.0f
						) { //if object has platform-like collision
							if (Engine::Physics::Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j) & Engine::Physics::CollisionType::BOTTOM && physx.current_Y_max_level > j) {
								physx.time_falling = 0.f;
								physx.fallingDistance = 0.f;
								physx.linear_velocity.y = 0.0f;
								tr.pos.y = float(j) + 1.0f;
								physx.current_Y_max_level = j + 1;
								physx.platform_collision = true;
							}
						}
						else if (static_cast<Engine::Object::BlockInfo*>(obj_info_ptr)->collision) //usual collision
							switch (Engine::Physics::Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j)) {
							case Engine::Physics::CollisionType::LEFT:
								if (delta_move.x < 0.0f) delta_move.x = 0.0f;
								physx.collision_mask |= Engine::Physics::CollisionType::LEFT;
								tr.pos.x = float(i) + 1.0f + entity->hitbox.size.x * 0.5f;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								break;
							case Engine::Physics::CollisionType::RIGHT:
								if (delta_move.x > 0.0f) delta_move.x = 0.0f;
								physx.collision_mask |= Engine::Physics::CollisionType::RIGHT;
								tr.pos.x = float(i) - entity->hitbox.size.x * 0.5f;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								break;
							case Engine::Physics::CollisionType::TOP:
								physx.collision_mask |= Engine::Physics::CollisionType::TOP;
								physx.time_falling = 0.f;
								physx.fallingDistance = 0.f;
								physx.linear_velocity.y = 0.0f;
								delta_move.y = 0.0f;
								tr.pos.y = float(j) - entity->hitbox.size.y;
								entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);
								break;
							case Engine::Physics::CollisionType::BOTTOM:
								if (physx.linear_velocity.y > 0.0f) continue;
								physx.collision_mask |= Engine::Physics::CollisionType::BOTTOM;
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

			tr.pos += delta_move * Engine::Util::TimeManager::deltaTime;
			entity->hitbox.center = tr.pos + glm::vec2(0.0f, entity->hitbox.size.y * 0.5f);

			if (true) {
				//show text about this mob (Name HP/MAX_HP)
			}
		}

		//update entity
		entity_base->update();

		//update render data
		Engine::Resource::Sprite& sprite = *entity_base->current_sprite;
		glm::vec2 sprite_adjustment(0.0f, sprite.base_size * sprite.ratio.y * 0.5f * tr.scale.y);
		Engine::Util::Math::update_2D_TRS(
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

	sb->update_data(entity_render_buf.data(), entities.size() * sizeof(EntityRenderData));
}

void Engine::Core::EntitySystem::render(std::unique_ptr<Engine::Graphics::Renderer>& renderer) const {
	renderer->bind_pipeline(*pipeline);
	renderer->renderInstancedData(*instance_vb, *instance_ib, 6U, entities.size());
}

void Engine::Core::EntitySystem::connect_world_data(
	WorldSlot* world_slots_ptr, uint32_t world_width, uint32_t world_height,
	std::unordered_map<uint32_t, std::unique_ptr<Engine::Object::ObjectComponent>>* obj_comps_ptr
) {
	this->world_slots_ptr = world_slots_ptr;
	this->world_width = world_width;
	this->world_height = world_height;
	this->obj_comps_ptr = obj_comps_ptr;
}

bool Engine::Core::EntitySystem::spawn_entity(uint32_t id, glm::vec2 pos) {
	uint32_t factory_id = Engine::Entity::EntityInfoManager::Instance().get_entity_info(id)->factory_ID;
	Engine::Entity::EntityFactoryI* factory = Engine::Entity::EntityFactoryRegistry::Instance().get_factory(factory_id);
	entities.emplace_back(factory->spawn(id, pos.x, pos.y));
	return true;
}

bool Engine::Core::EntitySystem::spawn_projectile() {
	return false;
}