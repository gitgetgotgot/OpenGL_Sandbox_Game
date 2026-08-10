#include <Entities/EntitySystem.h>
#include <Entities/Enemies/Slime.h>
#include <Entities/Enemies/Zombie.h>
#include <Entities/Enemies/FlyingEye.h>
#include <Entities/Projectile.h>
#include <IOSystem/SystemContext.h>
#include <Utility/TimeManager.h>
#include <Utility/Math.h>

void EntitySystem::init() {
	entities.reserve(MAX_ENTITIES_RENDER);
	entity_render_buf.reserve(MAX_ENTITIES_RENDER);

	entity_sp = std::make_unique<ShaderProgram>("Resources/shaders/sprites2.vert", "Resources/shaders/sprites.frag");

	entity_ssbo = std::make_unique<SSBO>();
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

void EntitySystem::update() {
	int size = entities.size();
	for (int i = 0; i < size; i++) {
		EntityBase* entity = entities[i].get();
		Transform& tr = entity->transform;
		EntityInfo* info = EntityDB::entityInfo[entity->entity_id].get();
		EntityRenderData& render_data = entity_render_buf[i];

		//update physics
		if (info->type == EntityType::isMob) {
			MobPhysics& physx = static_cast<Mob*>(entity)->physics;
			float width = entity->hitbox.size.x;
			float height = entity->hitbox.size.y;
			float x = entity->hitbox.center.x - width * 0.5f;
			float y = entity->hitbox.center.y - height * 0.5f;

			int min_slot_x = (int)x - 1;
			int max_slot_x = (int)(x + width) + 2;
			int min_slot_y = (int)y - 1;
			int max_slot_y = (int)(y + height) + 2;

			if (!(physx.collision & CollisionType::BOTTOM)) {
				physx.time_falling += TimeManager::deltaTime;
				if (physx.affected_by_gravity) {
					physx.linear_velocity.y -= MobPhysics::GRAVITY * physx.gravity_scale * TimeManager::deltaTime;
					physx.linear_velocity.y = std::max(physx.linear_velocity.y, -MobPhysics::MAX_FALL_SPEED);
				}
				float dY = physx.linear_velocity.y * TimeManager::deltaTime;
				entity->hitbox.center.y += dY;
				physx.fallingDistance += std::abs(dY);
				//calculate current Y max level where mob is falling down
				if (physx.linear_velocity.y < 0.0f)
					physx.current_Y_max_level = (int)entity->hitbox.center.y;
			}
			entity->hitbox.center.x += physx.linear_velocity.x * TimeManager::deltaTime;

			
			/*
			for (int i = min_slot_x; i < max_slot_x; i++) {
				for (int j = min_slot_y; j < max_slot_y; j++) {
					if (sprites_Array[i][j].object.object_type) {
						if (sprites_Array[i][j].object.object_type == isCompObjPart) {  //if part of complex object, then use its column and line
							object_id = sprites_Array[sprites_Array[i][j].object.component->get_column()][sprites_Array[i][j].object.component->get_line()].object.object_id;
						}
						else {  //if it's simple or complex object
							object_id = sprites_Array[i][j].object.object_id;
						}
						if (objectInfo[object_id]->allow_bottom_collision()) { //if only bottom collision is allowed
							if (Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j, BLOCK_VISIBLE_SIZE) == BOTTOM && physx.current_Y_max_level > j) {
								if (physx.current_jump_V && !physx.moving_down)
									continue;
								physx.time_falling = 0.f;
								entity->hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + height * 0.5f;
								physx.fallingDistance = 0.f;
								physx.current_jump_V = 0.f;
								physx.current_Y_max_level = j + 1;
								physx.has_bottom_collision_only_with_objects = true;
							}
						}
						else if (objectInfo[object_id]->allow_collision()) //if all types of collision are allowed
							switch (Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j, BLOCK_VISIBLE_SIZE)) {
							case LEFT:
								if (!(is_solid_block(i + 1, j))) {
									entity->hitbox.center.x = i * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + width * 0.5f;
									physx.has_side_collision = true;
									if (eXinc < 0.f) eXinc = 0.f;
								}
								break;
							case RIGHT:
								entity->hitbox.center.x = i * BLOCK_VISIBLE_SIZE - width * 0.5f;
								physx.has_side_collision = true;
								if (eXinc > 0.f) eXinc = 0.f;
								break;
							case TOP:
								physx.has_top_collision = true;
								physx.time_falling = 0.f;
								entity->hitbox.center.y = j * BLOCK_VISIBLE_SIZE - height * 0.5f;
								physx.fallingDistance = 0.f;
								physx.current_jump_V = 0.f;
								break;
							case BOTTOM:
								has_bottom = true;
								physx.time_falling = 0.f;
								entity->hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + height * 0.5f;
								physx.fallingDistance = 0.f;
								physx.current_jump_V = 0.f;
								physx.current_Y_max_level = j + 1;
								break;
							case CORNER:
								break;
							default:
								break;
							}
					}
				}
			}
			if (has_bottom) {
				physx.has_bottom_collision = true;
				physx.has_bottom_collision_only_with_objects = false;
			}
			else physx.has_bottom_collision = false;
			if (physx.has_bottom_collision_only_with_objects)
				physx.has_bottom_collision = true;

			if (physx.should_jump) { //start jump
				physx.should_jump = false;
				physx.has_bottom_collision = false;
				physx.has_bottom_collision_only_with_objects = false;
				physx.current_jump_V = physx.jump_V0 * BLOCK_VISIBLE_SIZE;
			}
			entity->hitbox.center.x += eXinc;
			physx.Xinc = eXinc;
			*/

			if (GameMath::mouse_intersection(
				glm::vec2(x, y), glm::vec2(width, height), glm::vec2(SystemContext::mouse.world_x_pos, SystemContext::mouse.world_y_pos))) {
				//show text about this mob (Name HP/MAX_HP)
			}
		}

		//update entity
		entity->update();

		//update render data
		Sprite& sprite = *entity->current_sprite;
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
}

void EntitySystem::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	renderer->renderInstancedData(entity_sp, instance_vao, instance_vbo, instance_ebo, 6U, 9U);
}

bool EntitySystem::spawn_entity(uint32_t id, glm::vec2 pos) {
	std::unique_ptr<EntityInfo>& entityInfo = EntityDB::entityInfo[id];
	/*	
	if (entityInfo->type == EntityType::isMob) {
		switch (entityInfo->type) {
		case EnemyType::isSlime: {
			entities.emplace_back(std::make_unique<Slime>(id, pos.x, pos.y, *entityInfo));
		}
		case EnemyType::isZombie: {
			entities.emplace_back(std::make_unique<Zombie>(id, pos.x, pos.y, *entityInfo));
		}
		case EnemyType::isFlyingEye: {
			entities.emplace_back(std::make_unique<FlyingEye>(id, pos.x, pos.y, *entityInfo));
		}
		}
	}
	else if (entityInfo->type == EntityType::isProjectile) {
		//entities.emplace_back(std::make_unique<GravityProjectile>(id, pos.x, pos.y, *entityInfo));
	}*/
	return true;
}

bool EntitySystem::spawn_projectile() {
	return false;
}