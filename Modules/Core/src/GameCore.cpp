#define _CRT_SECURE_NO_WARNINGS

#include <Core/GameCore.h>
#include <Entities/Projectile.h>
#include <Entities/Enemies/Slime.h>
#include <Entities/Enemies/Zombie.h>
#include <Entities/Enemies/FlyingEye.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <glm/gtx/string_cast.hpp>

bool Game::update() {
	timeMgr.update();

	SystemContext::mouse.get_mouse_ortho_coords(SystemContext::screen);

	if (SystemContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
		return 0;
	}
	if (SystemContext::keyBoard.key_is_pressed(Key::KeyF)) {
		toggle_Fullscreen();
	}

	world->update();
	ui_renderer->update();

	return 1;
	/*switch (game_update_state) {
	case inMainMenu: {
		game_render_state = Game_State::inMainMenu;
		ColorVertex2f* ptr = buttons_buffer;
		buttons_amount = 3;
		for (int i = 0; i < 3; i++) {
			if (main_buttons[i].cursor_is_inside(mouse.mouseX, mouse.mouseY) && mouse.left_button) {
				mouse.left_button = false;
				if (i == 0) {
					game_update_state = Game_State::inWorldExplorer;
					break;
				}
				else if (i == 1) {
					game_update_state = Game_State::inOptions;
					break;
				}
				else if (i == 2) {
					return 0;
					break;
				}
			}
			main_buttons[i].update_buffer(ptr);
		}
		return 1;
	}
	case inWorldExplorer: {
		game_render_state = Game_State::inWorldExplorer;
		if (keyStates[GLFW_KEY_ESCAPE]) {
			keyStates[GLFW_KEY_ESCAPE] = false;
			game_update_state = Game_State::inMainMenu;
			return 1;
		}
		ColorVertex2f* ptr = buttons_buffer;
		buttons_amount = 2;
		for (int i = 3; i < 5; i++) {
			if (main_buttons[i].cursor_is_inside(mouse.mouseX, mouse.mouseY) && mouse.left_button) {
				mouse.left_button = false;
				if (i == 3 && !active_world.empty()) {
					loading_the_world = true;
					game_update_state = Game_State::WorldIsLoading;
					std::thread(&Game::load_the_world_thread, this).detach();
					break;
				}
				else if (i == 4) {
					game_update_state = Game_State::inWorldCreator;
					break;
				}
			}
			main_buttons[i].update_buffer(ptr);
		}
		for (auto& button : world_buttons) {
			button.update_pressed_state(mouse);
			if (button.isActive) {
				active_world = button.text;
			}
			button.update_buffer(ptr);
			buttons_amount++;
		}
		return 1;
	}
	case inWorldCreator: {
		game_render_state = Game_State::inWorldCreator;
		if (keyStates[GLFW_KEY_ESCAPE]) {
			keyStates[GLFW_KEY_ESCAPE] = false;
			game_update_state = Game_State::inWorldExplorer;
			text_field.text = "";
			return 1;
		}
		ColorVertex2f* ptr = buttons_buffer;
		buttons_amount = 2;
		if (main_buttons[5].cursor_is_inside(mouse.mouseX, mouse.mouseY) && mouse.left_button && !text_field.text.empty()) {
			mouse.left_button = false;

			std::string fileName("Saves/");
			fileName += text_field.text;
			fileName += ".txt";
			active_world = fileName;
			save_Files.emplace_back(fileName);
			init_world_buttons();
			text_field.text = "";

			creating_the_world = true;
			game_update_state = Game_State::WorldIsCreating;
			std::thread(&Game::create_the_world_thread, this).detach();
			return 1;
		}
		main_buttons[5].update_buffer(ptr);
		text_field.update_for_fileName_input(mouse, keyStates);
		text_field.update_buffer(ptr);
		return 1;
	}
	case WorldIsCreating: {
		game_render_state = Game_State::WorldIsCreating;
		if (!creating_the_world) game_update_state = Game_State::inGame;
		return 1;
	}
	case WorldIsLoading: {
		game_render_state = Game_State::WorldIsLoading;
		if (!loading_the_world) game_update_state = Game_State::inGame;
		return 1;
	}
	case WorldIsSaving: {
		game_render_state = Game_State::WorldIsSaving;
		if (!saving_the_world) game_update_state = Game_State::inMainMenu;
		return 1;
	}
	case inGame: {
		game_render_state = Game_State::inGame;
		
		//move
		playerXinc = 0.f;
		playerYinc = 0.f;
		if (keyStates[GLFW_KEY_W]) {
			if (keyStates[GLFW_KEY_LEFT_SHIFT])
				playerYinc = player.speed * 5;
			else
				playerYinc = player.speed;
		}
		if (keyStates[GLFW_KEY_A]) {
			player.looks_at_left = true;
			if (keyStates[GLFW_KEY_LEFT_SHIFT])
				playerXinc = -player.speed * 5;
			else
				playerXinc = -player.speed;
		}
		if (keyStates[GLFW_KEY_D]) {
			player.looks_at_left = false;
			if (keyStates[GLFW_KEY_LEFT_SHIFT])
				playerXinc = player.speed * 5;
			else
				playerXinc = player.speed;
		}

		//[[TEST]] spawning entities
		if (keyStates[GLFW_KEY_LEFT_SHIFT]) {
			if (keyStates[GLFW_KEY_Z]) {
				keyStates[GLFW_KEY_Z] = false;
				entitySystem->spawn_entity(7, glm::vec2(GameContext::PLAYER_LAST_POS.x, GameContext::PLAYER_LAST_POS.y + GameContext::BLOCK_SIZE * 4));
			}
			if (keyStates[GLFW_KEY_S]) {
				keyStates[GLFW_KEY_S] = false;
				entitySystem->spawn_entity(6, glm::vec2(GameContext::PLAYER_LAST_POS.x, GameContext::PLAYER_LAST_POS.y + GameContext::BLOCK_SIZE * 4));
			}
			if (keyStates[GLFW_KEY_P]) {
				keyStates[GLFW_KEY_P] = false;
				entitySystem->spawn_entity(8, glm::vec2(GameContext::PLAYER_LAST_POS.x - GameContext::BLOCK_SIZE * 4, GameContext::PLAYER_LAST_POS.y + GameContext::BLOCK_SIZE * 4));
				entitySystem->spawn_entity(9, glm::vec2(GameContext::PLAYER_LAST_POS.x + GameContext::BLOCK_SIZE * 10, GameContext::PLAYER_LAST_POS.y + GameContext::BLOCK_SIZE * 13));
				entitySystem->spawn_entity(10, glm::vec2(GameContext::PLAYER_LAST_POS.x - GameContext::BLOCK_SIZE * 20, GameContext::PLAYER_LAST_POS.y + GameContext::BLOCK_SIZE * 9));
			}
		}

		//[[TEST]] crafting system
		if (keyStates[GLFW_KEY_C]) {
			keyStates[GLFW_KEY_C] = false;
			crafting_system.show_all_crafts = !crafting_system.show_all_crafts;
		}

		//check collision for player
		if (collisionIsOn) {
			player.moving_down = false;
			player.has_bottom_collision_only_with_objects = false;
			bool has_side = false;
			bool has_bottom = false;
			bool has_top = false;
			int leftX = (player.hitbox.center.x - player.hitbox.size.x * 0.5f) / BLOCK_VISIBLE_SIZE - 1;
			if (leftX < 0) leftX = 0;
			int rightX = leftX + 3;
			if (rightX >= world_width) rightX = world_width - 1;
			int bottomY = (player.hitbox.center.y - player.hitbox.size.y * 0.5f) / BLOCK_VISIBLE_SIZE - 1;
			if (bottomY < 0) bottomY = 0;
			int topY = bottomY + 4;
			if (topY >= world_height) topY = world_height - 1;

			if (!player.has_bottom_collision) { //if the player doesn't have bottom collision, then he is falling
				//add delta time to time in free falling
				player.time_in_free_falling += deltaTime;
				//calculate distance with this time_in_free_falling between last and current distances and add to current player Y
				float distance = player.jump_V0 * player.time_in_free_falling - BLOCK_VISIBLE_SIZE * 2 * 9.8f * player.time_in_free_falling * player.time_in_free_falling / 2;
				float dY = distance - player.fallingDistance;
				player.hitbox.center.y += dY;
				//if the player has jumped, then calculate the current Y level to later get the level where it stops and starts going down
				if (player.jump_V0 && dY >= 0.f)
					player.current_Y_max_level = (player.hitbox.center.y - player.hitbox.size.y * 0.5f) / BLOCK_VISIBLE_SIZE;
				//check whether the player is moving down or up based on positive/negative delta distance
				if (dY < 0.f)
					player.moving_down = true;
				//adjust camera
				camera.dY -= dY;
				//remember current distance while moving
				player.fallingDistance = distance;
			}
			//check player collision with blocks
			int object_id;
			for (int i = leftX; i <= rightX; i++) {
				for (int j = bottomY; j <= topY; j++) {
					if (sprites_Array[i][j].object.object_type) {
						if (sprites_Array[i][j].object.object_type == isCompObjPart) {  //if part of complex object, then use its column and line
							object_id = sprites_Array[sprites_Array[i][j].object.component->get_column()][sprites_Array[i][j].object.component->get_line()].object.object_id;
						}
						else {  //if it's simple or complex object
							object_id = sprites_Array[i][j].object.object_id;
						}
						if (objectInfo[object_id]->allow_bottom_collision()) { //if only bottom collision is allowed
							if (Collisions::getTypeCollisionAABBwithBlock(player.hitbox, i, j, BLOCK_VISIBLE_SIZE) == BOTTOM && player.current_Y_max_level > j) {
								if (player.jump_V0 && !player.moving_down)
									continue;
								if (playerYinc < 0.f) playerYinc = 0.f;
								player.time_in_free_falling = 0.f;
								player.hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + player.hitbox.size.y * 0.5f;
								player.fallingDistance = 0.f;
								player.jump_V0 = 0.f;
								player.current_Y_max_level = j + 1;
								player.has_bottom_collision_only_with_objects = true;
							}
						}
						else if (objectInfo[object_id]->allow_collision()) //if all types of collision are allowed
							switch (Collisions::getTypeCollisionAABBwithBlock(player.hitbox, i, j, BLOCK_VISIBLE_SIZE)) {
							case LEFT:
								if (!(is_solid_block(i + 1, j))) {
									if (playerXinc < 0.f) playerXinc = 0.f;
									player.hitbox.center.x = i * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + player.hitbox.size.x * 0.5f;
									has_side = true;
								}
								break;
							case RIGHT:
								if (playerXinc > 0.f) playerXinc = 0.f;
								player.hitbox.center.x = i * BLOCK_VISIBLE_SIZE - player.hitbox.size.x * 0.5f;
								has_side = true;
								break;
							case TOP:
								if (playerYinc > 0.f) playerYinc = 0.f;
								has_top = true;
								player.time_in_free_falling = 0.f;
								player.hitbox.center.y = j * BLOCK_VISIBLE_SIZE - player.hitbox.size.y * 0.5f;
								player.fallingDistance = 0.f;
								player.jump_V0 = 0.f;
								break;
							case BOTTOM:
								if (playerYinc < 0.f) playerYinc = 0.f;
								has_bottom = true;
								player.time_in_free_falling = 0.f;
								player.hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + player.hitbox.size.y * 0.5f;
								player.fallingDistance = 0.f;
								player.jump_V0 = 0.f;
								player.current_Y_max_level = j + 1;
								break;
							case CORNER:
								if (playerXinc != 0.f && playerYinc != 0.f) playerYinc = 0.f;
								break;
							default:
								break;
							}
					}
				}
			}
			if (has_bottom) {
				player.has_bottom_collision = true;
				player.has_bottom_collision_only_with_objects = false;
			}
			else player.has_bottom_collision = false;
			if (has_side) {
				player.has_side_collision = true;
			}
			else player.has_side_collision = false;
			if (has_top) player.has_top_collision = true;
			else player.has_top_collision = false;
			if (player.has_bottom_collision_only_with_objects)
				player.has_bottom_collision = true;

			if (player.has_side_collision && keyStates[GLFW_KEY_D] && player.has_bottom_collision) { //auto "jump" on one block, if going right and it is possible
				if (sprites_Array[rightX][bottomY + 1].object.object_type &&
					!sprites_Array[rightX][bottomY + 2].object.object_type &&
					!sprites_Array[rightX][bottomY + 3].object.object_type &&
					!sprites_Array[rightX][bottomY + 4].object.object_type &&
					!sprites_Array[rightX - 1][bottomY + 4].object.object_type &&
					!sprites_Array[rightX - 2][bottomY + 4].object.object_type) {
					player.hitbox.center.y = (bottomY + 1) * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + player.hitbox.size.y * 0.5f;
					camera.dY -= BLOCK_VISIBLE_SIZE;
					playerXinc = 5.f;
				}
			}
			if (player.has_side_collision && keyStates[GLFW_KEY_A] && player.has_bottom_collision) { //auto "jump" on one block, if going left and it is possible
				if (sprites_Array[leftX][bottomY + 1].object.object_type &&
					!sprites_Array[leftX][bottomY + 2].object.object_type &&
					!sprites_Array[leftX][bottomY + 3].object.object_type &&
					!sprites_Array[leftX][bottomY + 4].object.object_type &&
					!sprites_Array[leftX + 1][bottomY + 4].object.object_type &&
					!sprites_Array[leftX + 2][bottomY + 4].object.object_type) {
					player.hitbox.center.y = (bottomY + 1) * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + player.hitbox.size.y * 0.5f;
					camera.dY -= BLOCK_VISIBLE_SIZE;
					playerXinc = -5.f;
				}
			}
			if (keyStates[GLFW_KEY_SPACE] && player.has_bottom_collision && !player.has_top_collision) { //if has bottom collision, then can jump
				player.has_bottom_collision = false;
				player.jump_V0 = player.jump_speed;
			}
			if (keyStates[GLFW_KEY_S] && player.has_bottom_collision_only_with_objects) {
				player.has_bottom_collision = false;
				player.has_bottom_collision_only_with_objects = false;
				player.current_Y_max_level--; //make it lower by one so that when going throught the object it won't count bottom collision
			}
			player.sprite_time += deltaTime;
			if (player.has_bottom_collision || player.has_bottom_collision_only_with_objects) {
				if (playerXinc != 0.f) {
					if (player.current_sprite == 0)
						player.current_sprite = 2;
					else if (player.sprite_time >= 0.2f) {
						player.sprite_time = 0.f;
						player.current_sprite++;
						if (player.current_sprite > 5)
							player.current_sprite = 2;
					}
				}
				else
					player.current_sprite = 0;
			}
			else {
				player.current_sprite = 1;
			}
		}
		player.hitbox.center.x += playerXinc * deltaTime * player.stats.speedFactor;
		player.hitbox.center.y += playerYinc * deltaTime * player.stats.speedFactor;
		if (player.hitbox.center.x - player.hitbox.size.x * 0.5f < 0) player.hitbox.center.x = player.hitbox.size.x * 0.5f;
		if (player.hitbox.center.y - player.hitbox.size.y * 0.5f < 0) player.hitbox.center.y = player.hitbox.size.y * 0.5f;
		if (player.hitbox.center.x + player.hitbox.size.x * 0.5f > world_width * BLOCK_VISIBLE_SIZE) player.hitbox.center.x = world_width * BLOCK_VISIBLE_SIZE - player.hitbox.size.x * 0.5f;
		if (player.hitbox.center.y + player.hitbox.size.y * 0.5f > world_height * BLOCK_VISIBLE_SIZE) {
			player.time_in_free_falling = 0.f;
			player.fallingDistance = 0.f;
			player.jump_V0 = 0.f;
			player.hitbox.center.y = world_height * BLOCK_VISIBLE_SIZE - player.hitbox.size.y * 0.5f;
		}
		camera.dX = -player.hitbox.center.x + ScreenWidth / 2;
		camera.dY = -player.hitbox.center.y + ScreenHeight / 2;
		if (camera.dX > -camera.scalingDx) camera.dX = -camera.scalingDx;
		if (camera.dX < camera.rightBorderDx) camera.dX = camera.rightBorderDx;
		if (camera.dY > -camera.scalingDy) camera.dY = -camera.scalingDy;
		if (camera.dY < camera.topBorderDy) camera.dY = camera.topBorderDy;

		//update player effects
		player.stats.updateEffects(deltaTime);

		//All entities
		entities_count = 0;
		EntityRenderData* entity_buf = entity_sprite_buf;
		//player
		{
			player.sprite_left_down_corner.x = player.hitbox.center.x - player.sprite_size.x * 0.5f;
			player.sprite_left_down_corner.y = player.hitbox.center.y - player.hitbox.size.y * 0.5f;
			glm::mat4 matModel(1.f);
			matModel = glm::translate(matModel, glm::vec3(player.sprite_left_down_corner.x, player.sprite_left_down_corner.y, 0.f));
			matModel = glm::scale(matModel, glm::vec3(player.sprite_size.x, player.sprite_size.y, 0.f));
			entity_buf->modelMatrix = matModel;
			int offset = player.current_sprite * 4;
			if (player.looks_at_left) {
				entity_buf->tex_coords[0] = player.tex_coords[offset + 3];
				entity_buf->tex_coords[1] = player.tex_coords[offset + 2];
				entity_buf->tex_coords[2] = player.tex_coords[offset + 1];
				entity_buf->tex_coords[3] = player.tex_coords[offset];
				entity_buf->tex_id = 0.f;
			}
			else {
				entity_buf->tex_coords[0] = player.tex_coords[offset];
				entity_buf->tex_coords[1] = player.tex_coords[offset + 1];
				entity_buf->tex_coords[2] = player.tex_coords[offset + 2];
				entity_buf->tex_coords[3] = player.tex_coords[offset + 3];
				entity_buf->tex_id = 0.f;
			}
			entity_buf++;
			entities_count++;
		}

		//ambient sprites
		{
			SpriteData* ptr = ambient_sprite_buf;
			ambientController.updateAmbientLayer(isDay, cycle_time, day_ratio, ptr, ScreenWidth, ScreenHeight, BLOCK_VISIBLE_SIZE, deltaTime);
		}
		sprite_ambient_ssbo_p->bind_SSBO();
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SpriteData) * 12, ambient_sprite_buf);

		//view matrix
		viewMatrix = glm::mat4(1.f);
		//scale the camera
		viewMatrix = glm::scale(viewMatrix, glm::vec3(camera.scaling, camera.scaling, 0.f));
		//adjust camera based on the scaling (scales the camera in the middle, not from the corner) and change camera position based on camera dx and dy
		viewMatrix = glm::translate(viewMatrix, glm::vec3(camera.scalingDx + camera.dX, camera.scalingDy + camera.dY, 0.f));

		sprite_SP_ptr->activate_shader();
		sprite_SP_ptr->set_Uniform_Mat4("viewMatrix", viewMatrix);
		entity_sprite_SP_ptr->activate_shader();
		entity_sprite_SP_ptr->set_Uniform_Mat4("viewMatrix", viewMatrix);

		//update damage text
		int dmg_text_size = damage_text.size();
		for (int i = 0; i < dmg_text_size; i++) {
			if (damage_text[i].updateText(deltaTime, BLOCK_VISIBLE_SIZE)) {
				damage_text.erase(damage_text.begin() + i);
				dmg_text_size--;
				i--;
			}
		}

		//entities
		glm::vec2* tex_coords_ptr;
		entity_info_text.isActive = false;
		for (int e = 0; e < entities.size(); e++) {
			Smart_ptr<EntityBase>& entity = entities[e];
			float x = entity->hitbox.center.x - entity->hitbox.size.x * 0.5f;
			float y = entity->hitbox.center.y - entity->hitbox.size.y * 0.5f;
			float width = entity->hitbox.size.x;
			float height = entity->hitbox.size.y;
			//despawn any entity if it's more than 150 blocks away on X or Y axis from the player
			if (std::abs(x - player.hitbox.center.x) >= 150.f * BLOCK_VISIBLE_SIZE ||
				std::abs(y - player.hitbox.center.y) >= 150.f * BLOCK_VISIBLE_SIZE)
			{
				entities.erase(entities.begin() + e);
				e--;
				continue;
			}
			if (entity->get_HP() <= 0) {
				entity->do_entity_death_sound(audio_manager);
				drop_enemy_items(entity->entity_id, x, y + height / 2);
				entities.erase(entities.begin() + e);
				e--;
				continue;
			}
			//show info if mouse points on this entity's hitbox
			if (mouse.mouseX * (1 / camera.scaling) - camera.dX - camera.scalingDx >= x && mouse.mouseX * (1 / camera.scaling) - camera.dX - camera.scalingDx <= x + width &&
				mouse.mouseY * (1 / camera.scaling) - camera.dY - camera.scalingDy >= y && mouse.mouseY * (1 / camera.scaling) - camera.dY - camera.scalingDy <= y + height)
			{
				entity_info_text.isActive = true;
				int id = entity->entity_id;
				entity_info_text.info = entityInfo[id]->name + "(" + std::to_string(entity->get_HP()) + "/" + std::to_string(entityInfo[id]->get_HP()) + ")";
				entity_info_text.start_pos.x = mouse.mouseX - BLOCK_VISIBLE_SIZE * 1.5;
				entity_info_text.start_pos.y = mouse.mouseY;
			}

			int leftX = x / BLOCK_VISIBLE_SIZE - 1;
			int rightX = leftX + (int)(width - 0.1) + 2; //hmm
			int bottomY = y / BLOCK_VISIBLE_SIZE - 1;
			int topY = bottomY + (int)(height - 0.1) + 2; //hmm
			if (leftX < 0) leftX = 0;
			if (rightX >= world_width) rightX = world_width - 1;
			if (bottomY < 0) bottomY = 0;
			if (topY >= world_height) topY = world_height - 1;

			//update entity logic
			entity->update_entity(deltaTime, BLOCK_VISIBLE_SIZE, player.hitbox.center.x, player.hitbox.center.y);
			//entity sounds
			entity->do_entity_sounds(audio_manager);
			//physics
			EntityMovementType m_type = entity->get_movement_type();
			if (m_type == EntityMovementType::isWalking) {
				WalkingEnemyPhysics& physx = entity->get_walking_physics();
				physx.moving_down = false;
				physx.has_bottom_collision_only_with_objects = false;
				physx.has_side_collision = false;
				physx.has_top_collision = false;
				bool has_bottom = false;
				float eXinc = physx.Xinc;

				if (!physx.has_bottom_collision) {
					//add delta time to time in free falling
					physx.time_in_free_falling += deltaTime;
					//calculate distance with this time_in_free_falling between last and current distances and add to current player Y
					float distance = physx.current_jump_V * physx.time_in_free_falling - BLOCK_VISIBLE_SIZE * 2 * 9.8f * physx.time_in_free_falling * physx.time_in_free_falling / 2;
					float dY = distance - physx.fallingDistance;
					entity->hitbox.center.y += dY;
					//if the entity has jumped, then calculate the current Y level to later get the level where it stops and starts going down
					if (physx.current_jump_V && dY >= 0.f)
						physx.current_Y_max_level = entity->hitbox.center.y / BLOCK_VISIBLE_SIZE;
					//check whether the entity is moving down or up based on positive/negative delta distance
					if (dY < 0.f)
						physx.moving_down = true;
					//remember current distance while moving
					physx.fallingDistance = distance;
				}
				int object_id;
				for (int i = leftX; i <= rightX; i++) {
					for (int j = bottomY; j <= topY; j++) {
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
									physx.time_in_free_falling = 0.f;
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
									physx.time_in_free_falling = 0.f;
									entity->hitbox.center.y = j * BLOCK_VISIBLE_SIZE - height * 0.5f;
									physx.fallingDistance = 0.f;
									physx.current_jump_V = 0.f;
									break;
								case BOTTOM:
									has_bottom = true;
									physx.time_in_free_falling = 0.f;
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
			}
			else if (m_type == EntityMovementType::isFlying) {
				FlyingEnemyPhysics& physx = entity->get_flying_physics();
				float eXinc = physx.Xinc, eYinc = physx.Yinc;
				int object_id;
				for (int i = leftX; i <= rightX; i++) {
					for (int j = bottomY; j <= topY; j++) {
						if (sprites_Array[i][j].object.object_type) {
							if (sprites_Array[i][j].object.object_type == isCompObjPart) {  //if part of complex object, then use its column and line
								object_id = sprites_Array[sprites_Array[i][j].object.component->get_column()][sprites_Array[i][j].object.component->get_line()].object.object_id;
							}
							else {  //if it's simple or complex object
								object_id = sprites_Array[i][j].object.object_id;
							}
							if (objectInfo[object_id]->allow_collision()) //if all types of collision are allowed
								switch (Collisions::getTypeCollisionAABBwithBlock(entity->hitbox, i, j, BLOCK_VISIBLE_SIZE)) {
								case LEFT:
									entity->hitbox.center.x = i * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + width * 0.5f;
									if (eXinc < 0.f) eXinc = BLOCK_VISIBLE_SIZE * deltaTime * 4;
									break;
								case RIGHT:
									entity->hitbox.center.x = i * BLOCK_VISIBLE_SIZE - width * 0.5f;
									if (eXinc > 0.f) eXinc = -BLOCK_VISIBLE_SIZE * deltaTime * 4;
									break;
								case TOP:
									entity->hitbox.center.y = j * BLOCK_VISIBLE_SIZE - height * 0.5f;
									if (eYinc > 0.f) eYinc = 0.f;
									break;
								case BOTTOM:
									entity->hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + height * 0.5f;
									if (eYinc < 0.f) eYinc = 0.f;
									break;
								case CORNER:
									break;
								default:
									break;
								}
						}
					}
				}
				entity->hitbox.center.x += physx.Xinc;
				entity->hitbox.center.y += physx.Yinc;
				physx.Xinc = eXinc; physx.Yinc = eYinc;
			}
			else {

			}
			//update entity model
			entity->update_model(BLOCK_VISIBLE_SIZE);

			entity_buf->modelMatrix = entity->matModel;
			tex_coords_ptr = entityInfo[entity->entity_id]->get_tex_coords_ptr();
			int offset = 4 * entity->get_tex_index();
			if (entity->looks_at_left) {
				entity_buf->tex_coords[0] = tex_coords_ptr[offset + 3];
				entity_buf->tex_coords[1] = tex_coords_ptr[offset + 2];
				entity_buf->tex_coords[2] = tex_coords_ptr[offset + 1];
				entity_buf->tex_coords[3] = tex_coords_ptr[offset];
				entity_buf->tex_id = 0.f;
			}
			else {
				entity_buf->tex_coords[0] = tex_coords_ptr[offset];
				entity_buf->tex_coords[1] = tex_coords_ptr[offset + 1];
				entity_buf->tex_coords[2] = tex_coords_ptr[offset + 2];
				entity_buf->tex_coords[3] = tex_coords_ptr[offset + 3];
				entity_buf->tex_id = 0.f;
			}
			entity_buf++;
			entities_count++;

			//entity effects
			EntityStats& stats = entity->get_entity_stats();
			for (int i = 0; i < stats.effects.size(); i++) {
				Effect& effect = stats.effects[i];
				//update effect and remove if needed
				if (effect.updateEffect(deltaTime)) {
					stats.effects.erase(stats.effects.begin() + i);
					i--;
					continue;
				}
				if (effects[effect.id]->emitsParticles) {
					if (effects[effect.id]->emit_particle(particles_v, entity->hitbox.center, glm::vec2(width, height), deltaTime, BLOCK_VISIBLE_SIZE)) {
						Particle& particle = particles_v.back();
						ParticleInfo& info = particlesInfo[particle.id];
						if (info.emitsLight) {
							update_lighting(particle.sprite_center, info.light.light_color, info.light.light_radius* BLOCK_VISIBLE_SIZE, true);
						}
					}
				}
				if (effects[effect.id]->type == EffectType::isDamagingDebuff)
					effects[effect.id]->inflictEntityDamage(stats, effect.delta_dmg_time);
			}
		}
		//projectiles and active weapon
		
		//[[PROBABLY CAN ADD THIS RIGHT IN ENTITIES CYCLE
		if (active_weapon.isActive && active_weapon.hitboxIsActive) {
			for (int e = 0; e < entities.size(); e++) {
				Smart_ptr<EntityBase>& entity = entities[e];
				if (entity->get_entity_stats().hit_cd == 0.f && Collisions::checkCollision_AABB_with_OBB(entity->hitbox, active_weapon.hitbox)) {
					int damage = objectInfo[active_weapon.weapon_id]->get_damage();
					//crit damage
					if (1 + rand() % 100 <= objectInfo[active_weapon.weapon_id]->get_crit_chance()) {
						damage *= 2;
						damage_text.emplace_back(DamageText(glm::vec2(entity->hitbox.center.x, entity->hitbox.center.y),
							glm::vec4(1.f, 0.475f, 0.204f, 1.f), std::to_string(damage), BLOCK_VISIBLE_SIZE * 1.5f));
					}
					else {
						damage_text.emplace_back(DamageText(glm::vec2(entity->hitbox.center.x, entity->hitbox.center.y),
							glm::vec4(1.f, 0.906f, 0.78f, 1.f), std::to_string(damage), BLOCK_VISIBLE_SIZE));
					}
					entity->decrease_HP(damage);
					entity->do_entity_hit_sound(audio_manager);

					if (active_weapon.time_to_finish_swing < active_weapon.hit_cd)
						entity->get_entity_stats().hit_cd = active_weapon.time_to_finish_swing;
					else
						entity->get_entity_stats().hit_cd = active_weapon.hit_cd;
				}
			}
		}
		//]]

		for (int i = 0; i < projectiles.size(); i++) {
			Smart_ptr<EntityBase>& projectile = projectiles[i];
			int column = projectile->sprite_center_point.x / BLOCK_VISIBLE_SIZE;
			int line = projectile->sprite_center_point.y / BLOCK_VISIBLE_SIZE;
			Smart_ptr<EntityInfo>& projectileInfo = entityInfo[projectile->entity_id];
			//remove previous light
			if (projectileInfo->emitsLight) {
				LightComponent light = projectileInfo->light;
				update_lighting(projectile->hitbox.center, light.light_color, light.light_radius * BLOCK_VISIBLE_SIZE, false);
			}
			//if out of world
			if (column < 0 || column >= world_width || line < 0 || line >= world_height) {
				projectiles.erase(projectiles.begin() + i);
				i--;
				continue;
			}
			//if far away from player
			if (std::abs(projectile->hitbox.center.x - player.hitbox.center.x) >= 150.f * BLOCK_VISIBLE_SIZE ||  ///HHMMMMMMMMMMM
				std::abs(projectile->hitbox.center.y - player.hitbox.center.y) >= 150.f * BLOCK_VISIBLE_SIZE)
			{
				projectiles.erase(projectiles.begin() + i);
				i--;
				continue;
			}
			//check sollision with objects
			if (is_solid_block(column, line))
				if (Collisions::getTypeCollisionAABBwithBlock(projectile->hitbox, column, line, BLOCK_VISIBLE_SIZE)) {
					projectiles.erase(projectiles.begin() + i);
					i--;
					continue;
				}
			//check collision with entities
			bool next = false;
			for (int e = 0; e < entities.size(); e++) {
				Smart_ptr<EntityBase>& entity = entities[e];
				if (entity->get_entity_stats().hit_cd == 0.f && Collisions::checkCollisionAABB(projectile->hitbox, entity->hitbox)) {
					if (entity->get_HP() <= 0) {
						continue;
					}
					//entity takes damage
					int damage = projectile->get_proj_dmg();
					bool isCrit = projectile->dmg_is_crit();
					entity->decrease_HP(damage);
					entity->do_entity_hit_sound(audio_manager);
					//crit damage
					if (isCrit) {
						damage_text.emplace_back(DamageText(glm::vec2(entity->hitbox.center.x, entity->hitbox.center.y),
							glm::vec4(1.f, 0.475f, 0.204f, 1.f), std::to_string(damage), BLOCK_VISIBLE_SIZE * 1.5f));
					}
					else {
						damage_text.emplace_back(DamageText(glm::vec2(entity->hitbox.center.x, entity->hitbox.center.y),
							glm::vec4(1.f, 0.906f, 0.78f, 1.f), std::to_string(damage), BLOCK_VISIBLE_SIZE));
					}
					if (entityInfo[projectile->entity_id]->hasEffect) {
						EffectComponent effect = entityInfo[projectile->entity_id]->effect;
						EntityStats& stats = entity->get_entity_stats();
						apply_entity_effect(Effect(effect.duration, effect.id), stats);
					}
					//remove this projectile if no available hits left
					if (!projectile->update_proj_hits_counter()) {
						projectiles.erase(projectiles.begin() + i);
						i--;
						next = true;
						break;
					}
					entity->get_entity_stats().hit_cd = 0.5f;
				}
			}
			if (next) continue;
			projectile->update_entity(deltaTime, BLOCK_VISIBLE_SIZE, 0.f, 0.f);
			entity_buf->modelMatrix = projectile->matModel;
			tex_coords_ptr = entityInfo[projectile->entity_id]->get_tex_coords_ptr();
			if (projectile->looks_at_left) {
				entity_buf->tex_coords[0] = tex_coords_ptr[3];
				entity_buf->tex_coords[1] = tex_coords_ptr[2];
				entity_buf->tex_coords[2] = tex_coords_ptr[1];
				entity_buf->tex_coords[3] = tex_coords_ptr[0];
				entity_buf->tex_id = 0.f;
			}
			else {
				entity_buf->tex_coords[0] = tex_coords_ptr[0];
				entity_buf->tex_coords[1] = tex_coords_ptr[1];
				entity_buf->tex_coords[2] = tex_coords_ptr[2];
				entity_buf->tex_coords[3] = tex_coords_ptr[3];
				entity_buf->tex_id = 0.f;
			}
			entity_buf++;
			entities_count++;
			if (projectileInfo->emitsLight) {
				LightComponent light = projectileInfo->light;
				update_lighting(projectile->hitbox.center, light.light_color, light.light_radius * BLOCK_VISIBLE_SIZE, true);
			}
		}
		if (active_weapon.isActive) {
			entity_buf->modelMatrix = active_weapon.modelMatrix;
			if (active_weapon.render_upside_down) {
				entity_buf->tex_coords[0] = active_weapon.tex_coords[1];
				entity_buf->tex_coords[1] = active_weapon.tex_coords[0];
				entity_buf->tex_coords[2] = active_weapon.tex_coords[3];
				entity_buf->tex_coords[3] = active_weapon.tex_coords[2];
			}
			else {
				entity_buf->tex_coords[0] = active_weapon.tex_coords[0];
				entity_buf->tex_coords[1] = active_weapon.tex_coords[1];
				entity_buf->tex_coords[2] = active_weapon.tex_coords[2];
				entity_buf->tex_coords[3] = active_weapon.tex_coords[3];
			}
			entity_buf->tex_id = 0.f;
			entity_buf++;
			entities_count++;
		}
		//update particles
		
		//add particles to entities buffer
		int size = particles_v.size();
		for (int i = 0; i < size; i++) {
			tex_coords_ptr = particlesInfo[particles_v[i].id].tex_coords;
			entity_buf->modelMatrix = particles_v[i].modelMatrix;
			entity_buf->tex_coords[0] = tex_coords_ptr[0];
			entity_buf->tex_coords[1] = tex_coords_ptr[1];
			entity_buf->tex_coords[2] = tex_coords_ptr[2];
			entity_buf->tex_coords[3] = tex_coords_ptr[3];
			entity_buf->tex_id = 0.f;
			entity_buf++;
			entities_count++;
		}

		sprite_ssbo_p->bind_SSBO();
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(EntityRenderData)* entities_count, entity_sprite_buf);

		//physics update for dropped items
		for (int d = 0; d < dropped_items.size(); d++) {
			DroppedItem& item = dropped_items[d];
			float x = item.hitbox.center.x - item.hitbox.size.x * 0.5f;
			float y = item.hitbox.center.y - item.hitbox.size.y * 0.5f;
			//delete if out of world
			if (x > world_width * BLOCK_VISIBLE_SIZE || x < 0) {
				dropped_items.erase(dropped_items.begin());
				d--;
				continue;
			}
			//update falling for item if needed
			if (!item.has_bottom_collision) {
				item.time_in_free_falling += deltaTime;
				float distance = - BLOCK_VISIBLE_SIZE * 2 * 9.8f * item.time_in_free_falling * item.time_in_free_falling / 2;
				item.hitbox.center.y += distance - item.fallingDistance;
				item.fallingDistance = distance;
			}
			item.hitbox.center.x += item.Xinc * deltaTime;
			if (item.has_pick_cd)
				item.cd_time += deltaTime;
			if (item.has_pick_cd && item.cd_time >= 1.f)
				item.has_pick_cd = false;
			//get area of blocks to check
			int leftX = x / BLOCK_VISIBLE_SIZE - 1;
			int rightX = leftX + (int)(item.hitbox.size.x - 0.1) + 2; //hmm
			int bottomY = y / BLOCK_VISIBLE_SIZE - 1;
			int topY = bottomY + (int)(item.hitbox.size.y - 0.1) + 2; //hmm
			if (leftX < 0) leftX = 0;
			if (rightX >= world_width) rightX = world_width - 1;
			if (bottomY < 0) bottomY = 0;
			if (topY >= world_height) topY = world_height - 1;
			//check collisions
			int object_id;
			for (int i = leftX; i <= rightX; i++) {
				for (int j = bottomY; j <= topY; j++) {
					if (sprites_Array[i][j].object.object_type) {
						if (sprites_Array[i][j].object.object_type == isCompObjPart) {  //if part of complex object, then use its column and line
							object_id = sprites_Array[sprites_Array[i][j].object.component->get_column()][sprites_Array[i][j].object.component->get_line()].object.object_id;
						}
						else {  //if it's simple or complex object
							object_id = sprites_Array[i][j].object.object_id;
						}
						if (objectInfo[object_id]->allow_bottom_collision()) { //if only bottom collision is allowed
							if (Collisions::getTypeCollisionAABBwithBlock(item.hitbox, i, j, BLOCK_VISIBLE_SIZE) == BOTTOM) {
								item.time_in_free_falling = 0.f;
								item.hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + item.hitbox.size.y * 0.5f;
								item.fallingDistance = 0.f;
							}
						}
						else if (objectInfo[object_id]->allow_collision()) //if all types of collision are allowed
							switch (Collisions::getTypeCollisionAABBwithBlock(item.hitbox, i, j, BLOCK_VISIBLE_SIZE)) {
							case LEFT:
								if (!(is_solid_block(i + 1, j))) {
									item.hitbox.center.x = i * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + item.hitbox.size.x * 0.5f;
									item.Xinc = 0.f; //hmm
								}
								break;
							case RIGHT:
								item.hitbox.center.x = i * BLOCK_VISIBLE_SIZE - item.hitbox.size.x * 0.5f;
								item.Xinc = 0.f; //hmm
								break;
							case TOP:
								item.time_in_free_falling = 0.f;
								item.hitbox.center.y = j * BLOCK_VISIBLE_SIZE - item.hitbox.size.y * 0.5f;
								item.fallingDistance = 0.f;
								break;
							case BOTTOM:
								item.time_in_free_falling = 0.f;
								item.hitbox.center.y = j * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE + item.hitbox.size.y * 0.5f;
								item.fallingDistance = 0.f;
								item.Xinc = 0.f;
								break;
							case CORNER:
								break;
							default:
								break;
							}
					}
				}
			}
			//check if item collides with the player and try to pick it
			if (Collisions::checkCollisionAABB(player.hitbox, item.hitbox) && !item.has_pick_cd) {
				//if player picked the item and amount of entity item is 0, then delete it
				if (try_to_pick_item(item) && item.amount == 0) {
					dropped_items.erase(dropped_items.begin() + d);
					d--;
				}
			}
		}

		if (active_weapon.projectile_attack_current_cd >= 0)
			active_weapon.projectile_attack_current_cd -= deltaTime;
		if (active_weapon.isActive)
			update_active_weapon(deltaTime);
		if (!mouse.left_button && !active_weapon.isActive) {
			active_breakable_object.time_breaking = 0.f;
		}

		//break or put objects and walls, use weapons, tools, etc. with mouse left button
		if (mouse.left_button && inventory_array[active_bar_slot].object.type) {
			ObjectType type = inventory_array[active_bar_slot].object.type;
			if (type == isWeapon) { //if the player is holding a weapon (pickaxe, axe, etc.)
				Smart_ptr<ObjectInfo>& weapon = objectInfo[inventory_array[active_bar_slot].object.id];
				WeaponType weapon_type = weapon->get_weapon_type();
				//start weapon animation if not active
				if (!active_weapon.isActive) {
					activate_weapon(weapon_type);
				}
				if (weapon_type == isPickaxe) {
					//the first step is to calculate how many blocks based on cameraDx/y and dx/y, then how many blocks on the camera based on resolution and scaling with current mouse coordinates
					int line = (-camera.dY - camera.scalingDy) / BLOCK_VISIBLE_SIZE + mouse.mouseY / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
					int column = (-camera.dX - camera.scalingDx) / BLOCK_VISIBLE_SIZE + mouse.mouseX / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
					if (sprites_Array[column][line].object.object_type) {
						if (sprites_Array[column][line].object.object_type == isCompObjPart) {  //if is a part of complex object, then get its column and line
							int new_column = sprites_Array[column][line].object.component->get_column();
							int new_line = sprites_Array[column][line].object.component->get_line();
							column = new_column;
							line = new_line;
						}
						object_id = sprites_Array[column][line].object.object_id;
						//update or start breaking object
						if (active_breakable_object.isBreaking) {
							//if object is breaked then remove and drop it
							if (active_breakable_object.update_breaking_object(column, line, deltaTime)) {
								//drop item
								drop_item(objectInfo[object_id]->get_drop_object_id(), column * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE * 0.5f, line * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE * 0.5f, 1, 0.f, false, 0);
								//remove object
								if (sprites_Array[column][line].object.object_type == isBlock) {
									destroy_object(column, line);
								}
								else {
									destroy_complex_object(column, line);
								}
							}
						}
						else {
							active_breakable_object.start_breaking_object(column, line, objectInfo[object_id]->get_toughness() / (weapon->get_speed_factor() * weapon->get_instrument_power()));
						}
					}
				}
				else if (weapon_type == isAxe) {
					//the first step is to calculate how many blocks based on cameraDx/y and dx/y, then how many blocks on the camera based on resolution and scaling with current mouse coordinates
					int line = (-camera.dY - camera.scalingDy) / BLOCK_VISIBLE_SIZE + mouse.mouseY / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
					int column = (-camera.dX - camera.scalingDx) / BLOCK_VISIBLE_SIZE + mouse.mouseX / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
					//check if it is any kind of wood
					if (sprites_Array[column][line].object.object_type) {
						object_id = sprites_Array[column][line].object.object_id;
						if (sprites_Array[column][line].object.object_type == isBlock &&
							objectInfo[object_id]->get_block_type() == isWood ||
							objectInfo[object_id]->get_block_type() == isTreeTop)
						{
							if (active_breakable_object.isBreaking) {
								//if object is breaked then remove and drop it
								if (active_breakable_object.update_breaking_object(column, line, deltaTime)) {
									//drop item
									drop_item(objectInfo[object_id]->get_drop_object_id(), column * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE * 0.5f, line * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE * 0.5f, 1, 0.f, false, 0);
									//remove object
									sprites_Array[column][line].object = GameObject(None, 0);
								}
							}
							else {
								active_breakable_object.start_breaking_object(column, line, objectInfo[object_id]->get_toughness() / (weapon->get_speed_factor() * weapon->get_instrument_power()));
							}
						}
					}
				}
				else if (weapon_type == isHammer) {
					//the first step is to calculate how many blocks based on cameraDx/y and dx/y, then how many blocks on the camera based on resolution and scaling with current mouse coordinates
					int line = (-camera.dY - camera.scalingDy) / BLOCK_VISIBLE_SIZE + mouse.mouseY / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
					int column = (-camera.dX - camera.scalingDx) / BLOCK_VISIBLE_SIZE + mouse.mouseX / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
					if (sprites_Array[column][line].wall_id) { //if there is a wall in this slot
						if (active_breakable_object.isBreaking) {
							//if object is breaked then remove and drop it
							if (active_breakable_object.update_breaking_object(column, line, deltaTime)) {
								//drop item
								drop_item(sprites_Array[column][line].wall_id, column * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE * 0.5f, line * BLOCK_VISIBLE_SIZE + BLOCK_VISIBLE_SIZE * 0.5f, 1, 0.f, false, 0);
								//remowe wall
								destroy_wall(column, line);
							}
						}
						else {
							active_breakable_object.start_breaking_object(column, line, objectInfo[object_id]->get_toughness() / (weapon->get_speed_factor() * weapon->get_instrument_power()));
						}
					}
				}
			}
			else if (type == isWall) { //if the player is holding a wall, then he can put it if it is possible
				//the first step is to calculate how many blocks based on cameraDx/y and dx/y, then how many blocks on the camera based on resolution and scaling with current mouse coordinates
				int line = (-camera.dY - camera.scalingDy) / BLOCK_VISIBLE_SIZE + mouse.mouseY / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
				int column = (-camera.dX - camera.scalingDx) / BLOCK_VISIBLE_SIZE + mouse.mouseX / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
				if (!sprites_Array[column][line].wall_id) { //if there is no wall id (means that it is equal to 0 and there is no wall in this slot) 
					//if there is no wall then put new wall there
					set_wall(column, line, inventory_array[active_bar_slot].object.id);
					remove_inventory_item(active_bar_slot);
				}
			}
			else if (type == isConsumable || type == isPotion) {
				mouse.left_button = false;
				use_item_with_effect(active_bar_slot);
				audio_manager->play_Sound(1);
			}
			else { //if the player is holding a simple or complex object, then he can put it if it is possible
				int line = (-camera.dY - camera.scalingDy) / BLOCK_VISIBLE_SIZE + mouse.mouseY / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
				int column = (-camera.dX - camera.scalingDx) / BLOCK_VISIBLE_SIZE + mouse.mouseX / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
				if (!sprites_Array[column][line].object.object_type) {
					//if it's a simple object
					if (inventory_array[active_bar_slot].object.type == isBlock) {
						//check if there are any blocks near or a wall
						if (sprites_Array[column][line + 1].object.object_type || sprites_Array[column][line - 1].object.object_type ||
							sprites_Array[column + 1][line].object.object_type || sprites_Array[column - 1][line].object.object_type || sprites_Array[column][line].wall_id)
						{
							object_id = inventory_array[active_bar_slot].object.id;
							//check if it's a platform or a torch and use suitable sprite based on how it should be placed
							if (objectInfo[object_id]->get_block_type() == isPlatform || objectInfo[object_id]->get_block_type() == isTorch) {
								if (column - 1 >= 0 && is_solid_block(column - 1, line))
									object_id += 1;
								else if (column + 1 < world_width && is_solid_block(column + 1, line))
									object_id += 2;
							}
							set_block(column, line, object_id);
							remove_inventory_item(active_bar_slot);
						}
					}
					//if it's a complex object
					else if (inventory_array[active_bar_slot].object.type == isComplexObject) {
						bool enoughSpace = true;
						object_id = inventory_array[active_bar_slot].object.id;
						int width = column + objectInfo[object_id]->get_sizeX(), height = line + objectInfo[object_id]->get_sizeY();
						//check if enough space to put the object
						for (int i = column; i < width; i++)
							for (int j = line; j < height; j++) {
								if (sprites_Array[i][j].object.object_type) enoughSpace = false;
							}
						if (enoughSpace) {  //if enough space, then can put the object
							mouse.left_button = false;
							ComplexObjectType type = objectInfo[object_id]->get_comp_obj_type();
							if (type == isChest)
								sprites_Array[column][line].object = GameObject(isComplexObject, object_id, new ChestComponent);
							else if (type == isDoor)
								sprites_Array[column][line].object = GameObject(isComplexObject, object_id, new DoorComponent(0));
							else
								sprites_Array[column][line].object = GameObject(isComplexObject, object_id);

							//fill the rest space with "parts of the object"
							for (int i = column; i < width; i++)
								for (int j = line; j < height; j++) {
									if (!sprites_Array[i][j].object.object_type)
										sprites_Array[i][j].object = GameObject(isCompObjPart, 0, new ComplexObjectPartComponent(column, line));
								}
							if (objectInfo[object_id]->emitsLight) {
								update_lighting(glm::vec2((column + objectInfo[object_id]->get_sizeX() * 0.5f) * BLOCK_VISIBLE_SIZE, (line + objectInfo[object_id]->get_sizeY() * 0.5f) * BLOCK_VISIBLE_SIZE),
									objectInfo[object_id]->light_color, objectInfo[object_id]->light_radius * BLOCK_VISIBLE_SIZE, true);
							}

							remove_inventory_item(active_bar_slot);
						}
					}
				}
				else if (objectInfo[inventory_array[active_bar_slot].object.id]->effectId != -1) {
					mouse.left_button = false;
					use_item_with_effect(active_bar_slot);
				}
			}
		}
		//use specific objects (chests, doors, etc.) or throw items with mouse right button
		if (mouse.right_button) {
			mouse.right_button = false;
			if (currently_moving_object.object.type) { //SHOULD MAKE IT BETTER!!!!
				if (player.looks_at_left) {
					drop_item(currently_moving_object.object.id, player.hitbox.center.x,
						player.hitbox.center.y + player.hitbox.size.y * 0.25, currently_moving_object.amount, -3 * BLOCK_VISIBLE_SIZE, true, 1.f);
				}
				else {
					drop_item(currently_moving_object.object.id, player.hitbox.center.x,
						player.hitbox.center.y + player.hitbox.size.y * 0.25, currently_moving_object.amount, 3 * BLOCK_VISIBLE_SIZE, true, 1.f);
				}
				currently_moving_object.object = ItemObject(None, 0);
				currently_moving_object.amount = 0;
			}
			int line = (-camera.dY - camera.scalingDy) / BLOCK_VISIBLE_SIZE + mouse.mouseY / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
			int column = (-camera.dX - camera.scalingDx) / BLOCK_VISIBLE_SIZE + mouse.mouseX / BLOCK_VISIBLE_SIZE * (1 / camera.scaling);
			if (sprites_Array[column][line].object.object_type) {
				if (sprites_Array[column][line].object.object_type == isCompObjPart) {  //if is a part of complex object, then get its column and line
					int new_column = sprites_Array[column][line].object.component->get_column();
					int new_line = sprites_Array[column][line].object.component->get_line();
					column = new_column;
					line = new_line;
				}
				object_id = sprites_Array[column][line].object.object_id;
				if (sprites_Array[column][line].object.object_type == isComplexObject) {
					if (objectInfo[object_id]->get_comp_obj_type() == isChest) { //open/close chest
						if (active_chest.isOpen) {
							active_chest.isOpen = false;
							active_chest.slot_pointer = nullptr;
							inventoryIsOpen = false;
							//crafting_system.show_all_crafts = false;
						}
						else {
							active_chest.isOpen = true;
							active_chest.slot_pointer = sprites_Array[column][line].object.component->get_chest_slots();
							inventoryIsOpen = true;
							///crafting_system.show_all_crafts = false;
						}
					}
					else if (objectInfo[object_id]->get_comp_obj_type() == isDoor) { //open/close door
						int door_state = sprites_Array[column][line].object.component->get_door_state();
						if (!door_state) { //door is closed
							if (player.looks_at_left) {  //open to left
								if (!sprites_Array[column - 1][line].object.object_type && !sprites_Array[column - 1][line + 1].object.object_type &&
									!sprites_Array[column - 1][line + 2].object.object_type) {  //if enough space, then can open the door
									int width = column + objectInfo[object_id]->get_sizeX(), height = line + objectInfo[object_id]->get_sizeY();
									for (int i = column; i < width; i++)
										for (int j = line; j < height; j++) {
											sprites_Array[i][j].object = GameObject(None, 0);
										}
									column--;
									width = column + objectInfo[object_id + 1]->get_sizeX(), height = line + objectInfo[object_id + 1]->get_sizeY();
									sprites_Array[column][line].object = GameObject(isComplexObject, object_id + 1, new DoorComponent(1));
									for (int i = column; i < width; i++)
										for (int j = line; j < height; j++) {
											if (!sprites_Array[i][j].object.object_type)
												sprites_Array[i][j].object = GameObject(isCompObjPart, 0, new ComplexObjectPartComponent(column, line));
										}
								}
							}
							else {  //open to right
								if (!sprites_Array[column + 1][line].object.object_type && !sprites_Array[column + 1][line + 1].object.object_type &&
									!sprites_Array[column + 1][line + 2].object.object_type) {  //if enough space, then can open the door
									int width = column + objectInfo[object_id]->get_sizeX(), height = line + objectInfo[object_id]->get_sizeY();
									for (int i = column; i < width; i++)
										for (int j = line; j < height; j++) {
											sprites_Array[i][j].object = GameObject(None, 0);;
										}
									width = column + objectInfo[object_id + 2]->get_sizeX(), height = line + objectInfo[object_id + 2]->get_sizeY();
									sprites_Array[column][line].object = GameObject(isComplexObject, object_id + 2, new DoorComponent(2));
									for (int i = column; i < width; i++)
										for (int j = line; j < height; j++) {
											if (!sprites_Array[i][j].object.object_type)
												sprites_Array[i][j].object = GameObject(isCompObjPart, 0, new ComplexObjectPartComponent(column, line));
										}
								}
							}
							//check if enough space to open the door
							
						}
						else if (door_state == 1) {  //close to right
							int width = column + objectInfo[object_id]->get_sizeX(), height = line + objectInfo[object_id]->get_sizeY();
							for (int i = column; i < width; i++)
								for (int j = line; j < height; j++) {
									sprites_Array[i][j].object = GameObject(None, 0);
								}
							column++;
							width = column + objectInfo[object_id - 1]->get_sizeX(), height = line + objectInfo[object_id - 1]->get_sizeY();
							sprites_Array[column][line].object = GameObject(isComplexObject, object_id - 1, new DoorComponent(0));
							for (int i = column; i < width; i++)
								for (int j = line; j < height; j++) {
									if (!sprites_Array[i][j].object.object_type)
										sprites_Array[i][j].object = GameObject(isCompObjPart, 0, new ComplexObjectPartComponent(column, line));
								}
						}
						else {  //close to left
							int width = column + objectInfo[object_id]->get_sizeX(), height = line + objectInfo[object_id]->get_sizeY();
							for (int i = column; i < width; i++)
								for (int j = line; j < height; j++) {
									sprites_Array[i][j].object = GameObject(None, 0);
								}
							width = column + objectInfo[object_id - 2]->get_sizeX(), height = line + objectInfo[object_id - 2]->get_sizeY();
							sprites_Array[column][line].object = GameObject(isComplexObject, object_id - 2, new DoorComponent(0));
							for (int i = column; i < width; i++)
								for (int j = line; j < height; j++) {
									if (!sprites_Array[i][j].object.object_type)
										sprites_Array[i][j].object = GameObject(isCompObjPart, 0, new ComplexObjectPartComponent(column, line));
								}
						}
					}
				}
			}
		}

		return 1;
	}
	}*/
}

void Game::render() {
	renderer->clear(1.0, 1.0, 1.0);

	//atlas_texture.bind();
	world->render(renderer);
	ui_renderer->render(renderer);

	renderer->present();
	/*	
	switch (game_render_state) {
	case inMainMenu: {
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		ui_elements_vbo->bind_VBO();
		glBufferSubData(GL_ARRAY_BUFFER, 0, buttons_amount * 4 * sizeof(ColorVertex2f), buttons_buffer);

		color_ui_SP->activate_shader();
		ui_elements_vao->bind_VAO();
		glDrawElements(GL_TRIANGLES, buttons_amount * 6, GL_UNSIGNED_INT, 0);
		
		text_manager->add_centered_text_to_buffer("Main menu", ScreenHeight * 0.1, glm::vec2(ScreenWidth * 0.5, ScreenHeight * 0.9), glm::vec4(0.f, 1.f, 0.f, 1.f));
		text_manager->add_centered_text_to_buffer("version 1.0.0", ScreenHeight * 0.04, glm::vec2(ScreenWidth * 0.07, ScreenHeight * 0.03), glm::vec4(0.f, 1.f, 0.f, 1.f));
		for (int i = 0; i < 3; i++) {
			text_manager->add_centered_text_to_buffer(main_buttons[i].text.c_str(), ScreenHeight * 0.075, main_buttons[i].center_pos, glm::vec4(0.f, 1.f, 0.f, 1.f));
		}
		text_manager->render_text();

		break;
	}
	case inWorldExplorer: {
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		ui_elements_vbo->bind_VBO();
		glBufferSubData(GL_ARRAY_BUFFER, 0, buttons_amount * 4 * sizeof(ColorVertex2f), buttons_buffer);
		
		color_ui_SP->activate_shader();
		ui_elements_vao->bind_VAO();
		glDrawElements(GL_TRIANGLES, buttons_amount * 6, GL_UNSIGNED_INT, 0);

		for (int i = 3; i < 5; i++) {
			text_manager->add_centered_text_to_buffer(main_buttons[i].text.c_str(), ScreenHeight * 0.075, main_buttons[i].center_pos, glm::vec4(0.f, 1.f, 0.f, 1.f));
		}
		int size = world_buttons.size();
		for (int i = 0; i < size; i++) {
			text_manager->add_centered_text_to_buffer(world_buttons[i].text.c_str(), ScreenHeight * 0.05, world_buttons[i].center_pos, glm::vec4(0.f, 1.f, 0.f, 1.f));
		}
		text_manager->render_text();
		
		break;
	}
	case inWorldCreator: {
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		ui_elements_vbo->bind_VBO();
		glBufferSubData(GL_ARRAY_BUFFER, 0, buttons_amount * 4 * sizeof(ColorVertex2f), buttons_buffer);

		color_ui_SP->activate_shader();
		ui_elements_vao->bind_VAO();
		glDrawElements(GL_TRIANGLES, buttons_amount * 6, GL_UNSIGNED_INT, 0);

		text_manager->add_centered_text_to_buffer("World name:", ScreenHeight * 0.075, glm::vec2(ScreenWidth * 0.5, ScreenHeight * 0.6), glm::vec4(0.f, 1.f, 0.f, 1.f));
		text_manager->add_centered_text_to_buffer(main_buttons[5].text.c_str(), ScreenHeight * 0.075, main_buttons[5].center_pos, glm::vec4(0.f, 1.f, 0.f, 1.f));
		text_manager->add_centered_text_to_buffer(text_field.text.c_str(), ScreenHeight * 0.065, text_field.center_pos, glm::vec4(0.f, 0.f, 0.f, 1.f));
		text_manager->render_text();

		break;
	}
	case WorldIsCreating: {
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		text_manager->add_centered_text_to_buffer("Generating the world...", ScreenHeight * 0.1, glm::vec2(ScreenWidth * 0.5, ScreenHeight * 0.5), glm::vec4(0.f, 0.2f, 1.f, 1.f));
		text_manager->render_text();
		break;
	}
	case WorldIsLoading: {
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		text_manager->add_centered_text_to_buffer("Loading the world...", ScreenHeight * 0.1, glm::vec2(ScreenWidth * 0.5, ScreenHeight * 0.5), glm::vec4(0.f, 0.2f, 1.f, 1.f));
		text_manager->render_text();
		break;
	}
	case WorldIsSaving: {
		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		text_manager->add_centered_text_to_buffer("Saving the world...", ScreenHeight * 0.1, glm::vec2(ScreenWidth * 0.5, ScreenHeight * 0.5), glm::vec4(0.f, 0.2f, 1.f, 1.f));
		text_manager->render_text();
		break;
	}
	case inGame: {
		glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindTextureUnit(0, a_textures.get_texture_index(0));

		//0)ambient objects (sun, clouds, etc.)
		ambient_sprite_SP_ptr->activate_shader();
		instance_vao_p->bind_VAO();
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 12);

		//1) batch render all objects, items and walls
		sprite_SP_ptr->activate_shader();
		sprite_SP_ptr->set_uniform_float("day_ratio", day_ratio);
		sprite_vao_p->bind_VAO();
		glDrawElements(GL_TRIANGLES, index_size, GL_UNSIGNED_INT, 0);

		//2) instance render all entities(player, enemies, npc, projectiles, etc.)
			//[[hitbox
		entity_SP->activate_shader();
		entity_SP->set_Uniform_Mat4("modelMatrix", viewMatrix);
		entity_vao->bind_VAO();
		glDrawElements(GL_LINE_LOOP, 1 * 6, GL_UNSIGNED_INT, 0);
			//]]
		entity_sprite_SP_ptr->activate_shader();
		entity_sprite_SP_ptr->set_uniform_float("day_ratio", day_ratio);
		//should make common VAO for instanced rendering
		instance_vao_p->bind_VAO();
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, entities_count);

		//2.5 render damage text
		int size = damage_text.size();
		if (size) {
			text_manager->set_view_matrix(viewMatrix);
			for (DamageText& text : damage_text) {
				text_manager->add_centered_text_to_buffer(text.text.c_str(), text.text_height, text.start_pos, text.color);
			}
			text_manager->render_text();
			text_manager->set_view_matrix(glm::mat4(1.f));
		}

		//3) batch render all slots in inventory
		//4) batch render all sprites in inventory
		//5) batch render all text in inventory
		glBindTextureUnit(0, a_textures.get_texture_index(0));
		color_ui_SP->activate_shader();
		glm::mat4 model = glm::mat4(1.f);
		inventory_vao->bind_VAO();
		if (inventoryIsOpen) {  //draw inventory slots, inventory objects, text that contains amount of each object
			color_ui_SP->set_Uniform_Mat4("modelMatrix", model);
			glDrawElements(GL_TRIANGLES, (66 + additional_slots) * 6, GL_UNSIGNED_INT, 0);

			UI_sprite_SP_ptr->activate_shader();
			UI_sprite_SP_ptr->set_Uniform_Mat4("viewMatrix", model);
			UI_sprite_SP_ptr->set_uniform_float("day_ratio", 1.0);
			inventory_objects_vao->bind_VAO();
			glDrawElements(GL_TRIANGLES, inventory_index_size, GL_UNSIGNED_INT, 0);

			text_manager->update_text_array(inventory_text_info, inventory_text_size, ScreenHeight * 0.03, glm::vec4(1.0, 1.0, 1.0, 1.f));
			text_manager->render_text();
		}
		else {
			model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.f));
			//float dy = ScreenHeight * (1 / 0.8) - ScreenHeight;  = 0.25 * ScreenHeight
			model = glm::translate(model, glm::vec3(0.f, ScreenHeight * 0.25f, 0.f));
			color_ui_SP->set_Uniform_Mat4("modelMatrix", model);
			glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);

			UI_sprite_SP_ptr->activate_shader();
			UI_sprite_SP_ptr->set_Uniform_Mat4("viewMatrix", model);
			UI_sprite_SP_ptr->set_uniform_float("day_ratio", 1.0);
			inventory_objects_vao->bind_VAO();
			glDrawElements(GL_TRIANGLES, inventory_index_size, GL_UNSIGNED_INT, 0);

			text_manager->set_view_matrix(model);
			text_manager->update_text_array(inventory_text_info, inventory_text_size, ScreenHeight * 0.03, glm::vec4(1.0, 1.0, 1.0, 1.f));
			text_manager->render_text();
		}

		//6) draw object info box for the active slot or for the one that mouse cursor is pointing on
		//7) render info box text
		for (int i = 0; i < 2; i++) {  
			if (object_info_box[i].show_box) {
				text_manager->update_object_info_box_text(object_info_box[i].box_string.c_str(), ScreenHeight * 0.03, object_info_box[i].starting_pos, object_info_box[i].text_color, object_info_box[i].box_size);
				object_info_box[i].box_vertices[0] = object_info_box[i].box_vertices[6] = object_info_box[i].starting_pos.x;
				object_info_box[i].box_vertices[1] = object_info_box[i].box_vertices[19] = object_info_box[i].starting_pos.y - object_info_box[i].box_size[1];
				object_info_box[i].box_vertices[12] = object_info_box[i].box_vertices[18] = object_info_box[i].starting_pos.x + object_info_box[i].box_size[0];
				object_info_box[i].box_vertices[7] = object_info_box[i].box_vertices[13] = object_info_box[i].starting_pos.y;
				inventory_vbo->bind_VBO();
				glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * 4, object_info_box[i].box_vertices);
				color_ui_SP->activate_shader();
				inventory_vao->bind_VAO();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				text_manager->render_text();
				//break;
			}
		}
		//reset matrices
		text_manager->set_view_matrix(glm::mat4(1.f));
		color_ui_SP->activate_shader();
		color_ui_SP->set_Uniform_Mat4("modelMatrix", glm::mat4(1.f));

		//8)render the rest, but should rework this to minimize render calls and make a better system, for example render all things for UI in one call and all text for UI in one call
		std::stringstream ss;
		ss << "HP:" << player.stats.currentHP;
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.05, glm::vec2(ScreenWidth * 0.87, ScreenHeight * 0.95), glm::vec4(1.f, 0.f, 0.f, 1.f));

		ss.str("");
		ss << "MANA:" << player.stats.currentMANA;
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.05, glm::vec2(ScreenWidth * 0.87, ScreenHeight * 0.9), glm::vec4(0.f, 0.f, 1.f, 1.f));

		ss.str("");
		ss << "FPS:" << current_FPS;
		//ss << "DEF: " << player.stats.DEF;
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.04, glm::vec2(ScreenWidth * 0.75, ScreenHeight * 0.8), getRainbowColor(rainbow_color_time));

		ss.str("");
		ss << "C:" << int((ScreenWidth / 2 - camera.dX) / BLOCK_VISIBLE_SIZE) << "," << int((ScreenHeight / 2 - camera.dY) / BLOCK_VISIBLE_SIZE);
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.04, glm::vec2(ScreenWidth * 0.75, ScreenHeight * 0.75), getRainbowColor(rainbow_color_time));

		ss.str("");
		ss << "E:" << entities.size() << " P:" << projectiles.size() << "" << " D:" << dropped_items.size();
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.04, glm::vec2(ScreenWidth * 0.75, ScreenHeight * 0.7), getRainbowColor(rainbow_color_time));

		ss.str("");
		ss << "Time: " << int(cycle_time / 60) << ":" << int(cycle_time) % 60;
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.04, glm::vec2(ScreenWidth * 0.75, ScreenHeight * 0.65), getRainbowColor(rainbow_color_time));

		ss.str("");
		ss << "w.a. : " << glm::degrees(active_weapon.angle);
		text_manager->add_text_to_buffer(ss.str().c_str(), ScreenHeight * 0.04, glm::vec2(ScreenWidth * 0.75, ScreenHeight * 0.6), getRainbowColor(rainbow_color_time));

		if (entity_info_text.isActive) {
			text_manager->add_text_to_buffer(entity_info_text.info.c_str(), ScreenHeight * 0.02, entity_info_text.start_pos, glm::vec4(1.f, 1.f, 1.f, 1.f));
		}
		text_manager->render_text();

		break;
	}
	}*/
}

void Game::input_end_frame() {
	SystemContext::mouse.lb_prev = SystemContext::mouse.lb_curr;
	SystemContext::mouse.rb_prev = SystemContext::mouse.rb_curr;
	SystemContext::mouse.wheel_offset = 0;
	SystemContext::mouse.delta_x = 0;
	SystemContext::mouse.delta_y = 0;
	SystemContext::mouse.overlapped_by_UI_layer = false;

	for (int i = 0; i <= GLFW_KEY_LAST; i++) {
		SystemContext::keyBoard.keyStatesPrev[i] = SystemContext::keyBoard.keyStatesCurr[i];
	}

	SystemContext::keyBoard.currentPressedChars.clear();
}

void Game::toggle_Fullscreen() {
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (glfwGetWindowMonitor(window) == nullptr) {
		// Switch to full screen
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else {
		// Switch back to windowed mode with Full HD resolution
		glfwSetWindowMonitor(window, nullptr, 100, 100, 1920, 1080, GLFW_DONT_CARE);
	}
}

#define ADD_OBJECT_INFO(pointer) \
	ObjectsDB::objectInfo.emplace_back((pointer));

void Game::init() {
	srand(time(NULL));

	init_open_gl();
	init_input();

	//load save files if they exist
	//load_available_saves();

	//load textures
	atlas_texture.load_2D_texture("Resources/textures/2D_Textures.png", true);
	atlas_texture.bind(0);

	spriteMgr = SpriteManager::get_instance();
	ObjectsDB::objectInfo.reserve(102);

	//fill simple blocks info
	/*000*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(ObjectType::None, "Air"));
	/*001*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(ObjectType::isCompObjPart, "CompObjPart"));
	/*002*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Stone", isSolidBlock, 1, 2, true, false));
	/*003*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Dirt", isSolidBlock, 1, 3, true, false));
	/*004*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Grass Dirt", isSolidBlock, 1, 4, true, false));
	/*005*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Sand", isSolidBlock, 1, 5, true, false));
	/*006*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Oak", isWood, 1, 6, false, false));
	/*007*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Oak Planks", isSolidBlock, 1, 7, true, false));
	/*008*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Crimson Stone", isSolidBlock, 1, 8, true, false));
	/*009*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Corrupted Stone", isSolidBlock, 1, 9, true, false));
	/*010*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Copper Ore", isSolidBlock, 1, 10, true, false));
	/*011*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Iron Ore", isSolidBlock, 1, 11, true, false));
	/*012*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Gold Ore", isSolidBlock, 1, 12, true, false));
	/*013*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Water", isLiquid, 1, 13, false, false));
	/*014*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Snow", isSolidBlock, 1, 14, true, false));
	/*015*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Ice", isSolidBlock, 1, 15, true, false));
	/*016*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Snow Grass Dirt", isSolidBlock, 1, 16, true, false));
	/*017*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Crimson Grass Dirt", isSolidBlock, 1, 17, true, false));
	/*018*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Corrupted Grass Dirt", isSolidBlock, 1, 18, true, false));
	/*019*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Grass", isGrass, 1, 19, false, false));
	/*020*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Corrupted Grass", isGrass, 1, 20, false, false));
	/*021*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Crimson Grass", isGrass, 1, 21, false, false));
	/*022*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Day Flower", isFlower, 1, 22, false, false));
	/*023*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Torch", isTorch, 1, 23, false, false, 0, 1));
	/*024*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Torch(l)", isTorch, 1, 23, false, false, 0, 1));
	/*025*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Torch(r)", isTorch, 1, 23, false, false, 0, 1));
	/*026*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Ice Torch", isTorch, 1, 26, false, false, 0, 2));
	/*027*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Ice Torch(l)", isTorch, 1, 26, false, false, 0, 2));
	/*028*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Ice Torch(r)", isTorch, 1, 26, false, false, 0, 2));
	/*029*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Oak Platform", isPlatform, 1.f, 29, false, true));
	/*030*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Oak Platform(l)", isPlatform, 1.f, 29, false, true));
	/*031*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Oak Platform(r)", isPlatform, 1.f, 29, false, true));
	/*032*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Oak Top", isTreeTop, 1, 6, false, false));
	//fill walls info
	/*033*/ ADD_OBJECT_INFO(std::make_unique<WallInfo>("Stone Wall", 1.0f, 33));
	/*034*/ ADD_OBJECT_INFO(std::make_unique<WallInfo>("Dirt Wall", 1.0f, 34));
	/*035*/ ADD_OBJECT_INFO(std::make_unique<WallInfo>("Oak Wall", 1.0f, 35));
	/*036*/ ADD_OBJECT_INFO(std::make_unique<WallInfo>("Crimson Wall", 1.0f, 36));
	/*037*/ ADD_OBJECT_INFO(std::make_unique<WallInfo>("Corrupted Wall", 1.0f, 37));
	/*038*/ ADD_OBJECT_INFO(std::make_unique<WallInfo>("Snow Wall", 1.0f, 38));
	//fill complex objects info
	/*039*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Workbench", isWorkbench, 1, 38, 2, 1, false, true));
	/*040*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Furnace", isFurnace, 1, 39, 3, 2, false, false, 0, 3));
	/*041*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Anvil", isAnvil, 1, 40, 2, 1, false, false));
	/*042*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Oak Table", isTable, 1, 41, 3, 2, false, false));
	/*043*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Oak Chair", isChair, 1, 42, 1, 2, false, false)); //points to the left side
	/*044*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Oak Chair", isChair, 1, 42, 1, 2, false, false)); //points to the right side
	/*045*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Oak Door", isDoor, 1, 44, 1, 3, true, false)); //closed
	/*046*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Oak Door", isDoor, 1, 44, 2, 3, false, false)); //opened to the left side
	/*047*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Oak Door", isDoor, 1, 44, 2, 3, false, false)); //opened to the right side
	/*048*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Chest", isChest, 1, 47, 2, 2, false, false));
	//fill weapons info
	/*049*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Copper Pickaxe", isPickaxe, 1.f, 1, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*050*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Copper Axe", isAxe, 1.f, 1, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*051*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Copper Dagger", isDagger, 3, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*052*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Copper Sword", isSword, 4, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*053*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Copper Hammer", isHammer, 1.f, 2, 0.5f, 1.f, 0.f, -1, 2.f, 2.f, 5.f, false, 3));
	/*054*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Copper Bow", isBow, 3, 0.f, 15.f, 1.f, -1, 1.5f, 1.5f, 5.f, false, 0));
	/*055*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Iron Pickaxe", isPickaxe, 1.f, 1, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*056*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Iron Axe", isAxe, 1.f, 1, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*057*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Iron Dagger", isDagger, 3, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*058*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Iron Sword", isSword, 4, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*059*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Iron Hammer", isHammer, 1.f, 2, 0.5f, 1.f, 0.f, -1, 2.f, 2.f, 5.f, false, 3));
	/*060*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Iron Bow", isBow, 3, 0.f, 15.f, 1.f, -1, 1.5f, 1.5f, 5.f, false, 0));
	/*061*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Golden Pickaxe", isPickaxe, 1.f, 1, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*062*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Golden Axe", isAxe, 1.f, 1, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*063*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Golden Dagger", isDagger, 3, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*064*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Golden Sword", isSword, 4, 0.5f, 1.f, 0.f, -1, 1.5f, 1.5f, 5.f, false, 3));
	/*065*/ ADD_OBJECT_INFO(std::make_unique<InstrumentalWeaponInfo>("Golden Hammer", isHammer, 1.f, 2, 0.5f, 1.f, 0.f, -1, 2.f, 2.f, 5.f, false, 3));
	/*066*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Golden Bow", isBow, 3, 0.f, 30.f, 0.25f, -1, 1.5f, 1.5f, 5.f, false, 0));
	//fill coin info
	/*067*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isCoin, "Copper Coin"));
	/*068*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isCoin, "Silver Coin"));
	/*069*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isCoin, "Gold Coin"));
	/*070*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isCoin, "Platinum Coin"));
	//fill materials info
	/*071*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isMaterial, "Copper Ingot"));
	/*072*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isMaterial, "Iron Ingot"));
	/*073*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isMaterial, "Gold Ingot"));
	/*074*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isMaterial, "Gel"));
	//fill ammo info
	/*075*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Wooden Arrow", isArrow, 5, 1.5f, 1.5f, 0));
	/*076*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Flaming Arrow", isArrow, 7, 1.5f, 1.5f, 1, 1, 5));
	/*077*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Frostburn Arrow", isArrow, 9, 1.5f, 1.5f, 2, 2, 6));
	/*078*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Jester's Arrow", isArrow, 10, 1.5f, 1.5f, 3, 0, 7));
	/*079*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Unholy Arrow", isArrow, 14, 1.5f, 1.5f, 4, 0, 8));
	/*080*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Hellfire Arrow", isArrow, 16, 1.5f, 1.5f, 5, 0, 9));

	/*081*/ ADD_OBJECT_INFO(std::make_unique<ComplexObjectInfo>("Life Crystal", isLyfeCrystal, 1, 80, 2, 2, false, false, 3, 4));
	/*082*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Glass", isGlass, 0, 81, true, false));
	/*083*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Mushroom", isFlower, 0, 82, false, false));
	/*084*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Bottle", isBottle, 0, 83, false, false));
	/*085*/ ADD_OBJECT_INFO(std::make_unique<BlockInfo>("Bottle of water", isBottle, 0, 84, false, false));
	/*086*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isMaterial, "Lens"));
	/*087*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isMaterial, "Fallen Star"));
	/*088*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isConsumable, "Mana Crystal", 4, 0));
	/*089*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isPotion, "Healing Potion", 5, 0));
	/*090*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isPotion, "Speed Potion", 6, 0));
	/*091*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isPotion, "Defense Potion", 7, 0));
	/*092*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isPotion, "Regeneration Potion", 8, 0));
	/*093*/ ADD_OBJECT_INFO(std::make_unique<ObjectInfo>(isPotion, "Better Healing Potion", 9, 0));

	/*094*/ ADD_OBJECT_INFO(std::make_unique<MagicalWeaponInfo>("Magic wand", isMagical, 5, 5, 0.f, 5.f, 1.f, 0, 1.5f, 1.5f, 5.f, false, 11));
	/*095*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Shuriken", isThrowable, 10, 0.f, 10.f, 0.5f, 13, 1.5f, 1.5f, 5.f, true, 3));
	/*096*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Throwing knife", isThrowable, 5, 0.f, 10.f, 0.5f, 14, 1.5f, 1.5f, 5.f, false, 3));
	/*097*/ ADD_OBJECT_INFO(std::make_unique<MagicalWeaponInfo>("Game.cpp", isMagical, 0, 0, 0.f, 10.f, 0.5f, 11, 1.5f, 1.5f, 0.f, false, 14));
	/*098*/ ADD_OBJECT_INFO(std::make_unique<AmmoInfo>("Bullet", isBullet, 7, 1.f, 1.f, 12, 0, 10));
	/*099*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Pistol", isGun, 20, 0.f, 20.f, 0.5f, 0, 2.f, 1.f, 10.f, false, 12));
	/*100*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Shotgun", isShotgun, 15, 0.f, 20.f, 1.f, 0, 2.f, 1.f, 10.f, false, 13));
	/*101*/ ADD_OBJECT_INFO(std::make_unique<WeaponInfo>("Trident", isSword, 15, 0.25f, 1.5f, 0.f, 0, 2.5f, 2.5f, 10.f, false, 3));

	//effect and light components
	compsMgr = ComponentsManager::get_instance();
	
	//EFFECTS
	/*0 */ compsMgr->add_effect_component(0.f, 0); //empty
	/*1 */ compsMgr->add_effect_component(5.f, 5); // burning for 5 sec
	/*2 */ compsMgr->add_effect_component(5.f, 6); // frostburning for 5 sec
	/*3 */ compsMgr->add_effect_component(0.f, 0); // life crystall buff
	/*4 */ compsMgr->add_effect_component(0.f, 1); // mana crystall buff
	/*5 */ compsMgr->add_effect_component(0.f, 10); // heal potion buff
	/*6 */ compsMgr->add_effect_component(120.f, 3); // speed potion buff for 2 min
	/*7 */ compsMgr->add_effect_component(120.f, 2); // def potion buff for 2 min
	/*8 */ compsMgr->add_effect_component(120.f, 4); // regen potion buff for 2 min
	/*9 */ compsMgr->add_effect_component(0.f, 10);  // better heal potion buff
	
	//LIGHTS
	/*0 */compsMgr->add_light_component(0.f, glm::vec3{}); //empty
	/*1 */compsMgr->add_light_component(5.f, glm::vec3(1.f, 1.f, 0.f)); //torch
	/*2 */compsMgr->add_light_component(5.f, glm::vec3(0.f, 0.9f, 1.f)); //ice torch
	/*3 */compsMgr->add_light_component(3.f, glm::vec3(1.f, 1.f, 0.f)); //furnace
	/*4 */compsMgr->add_light_component(3.f, glm::vec3(0.8f, 0.2f, 0.2f)); //life crystal
	/*5 */compsMgr->add_light_component(3.f, glm::vec3(1.f, 1.f, 0.f)); //flame arrow
	/*6 */compsMgr->add_light_component(3.f, glm::vec3(0.f, 0.9f, 1.f)); //frostburn arrow
	/*7 */compsMgr->add_light_component(3.f, glm::vec3(1.f, 0.9f, 0.9f)); //jester's arrow
	/*8 */compsMgr->add_light_component(3.f, glm::vec3(0.306f, 0.086f, 0.702f)); //unholy arrow
	/*9 */compsMgr->add_light_component(3.f, glm::vec3(1.f, 0.76f, 0.03f)); //hellfire arrow
	/*10*/compsMgr->add_light_component(1.f, glm::vec3(0.5f, 0.f, 0.f)); //bullet
	/*11*/compsMgr->add_light_component(1.f, glm::vec3(0.506f, 0.286f, 0.902f)); //NULL projectile
	/*12*/compsMgr->add_light_component(0.5f, glm::vec3(1.f, 1.f, 0.f)); //flame particle
	/*13*/compsMgr->add_light_component(0.5f, glm::vec3(0.f, 0.9f, 1.f)); //frostburn particle

	const float block_UV_size = 1.f / 32.f;
	const float pixel_UV_size = 1.f / 512.f;
	SpriteManager::MAIN_PIXEL_UV_SIZE = pixel_UV_size;
	SpriteManager::MAIN_BLOCK_UV_SIZE = block_UV_size;
	//air
	spriteMgr->add_sprite();

	//UI sprites
	//inventory slot
	spriteMgr->add_sprite(block_UV_size * 2.f, 0.f, block_UV_size * 2.f, block_UV_size * 2.f);
	//chest slot
	spriteMgr->add_sprite(block_UV_size * 4.f, 0.f, block_UV_size * 2.f, block_UV_size * 2.f);
	//active hotbar slot
	spriteMgr->add_sprite(block_UV_size * 6.f, 0.f, block_UV_size * 2.f, block_UV_size * 2.f);
	//craft slot
	spriteMgr->add_sprite(block_UV_size * 8.f, 0.f, block_UV_size * 2.f, block_UV_size * 2.f);
	//craft slots border 1
	spriteMgr->add_sprite(block_UV_size * 10.f, 0.f, block_UV_size * 4.f, block_UV_size);
	//craft slots border 2
	spriteMgr->add_sprite(block_UV_size * 10.f, block_UV_size, block_UV_size * 4.f, block_UV_size);
	//tooltip
	spriteMgr->add_sprite(0.f, 0.f, pixel_UV_size * 4.f, pixel_UV_size * 4.f); //left bottom
	spriteMgr->add_sprite(0.f, pixel_UV_size * 4.f, pixel_UV_size * 4.f, pixel_UV_size * 12.f); //left center
	spriteMgr->add_sprite(0.f, pixel_UV_size * 16.f, pixel_UV_size * 4.f, pixel_UV_size * 4.f); //left top
	spriteMgr->add_sprite(pixel_UV_size * 4.f, 0.f, pixel_UV_size * 24.f, pixel_UV_size * 4.f); //center bottom
	spriteMgr->add_sprite(pixel_UV_size * 4.f, pixel_UV_size * 4.f, pixel_UV_size * 24.f, pixel_UV_size * 12.f); //center
	spriteMgr->add_sprite(pixel_UV_size * 4.f, pixel_UV_size * 16.f, pixel_UV_size * 24.f, pixel_UV_size * 4.f); //center top
	spriteMgr->add_sprite(pixel_UV_size * 28.f, 0.f, pixel_UV_size * 4.f, pixel_UV_size * 4.f); //right bottom
	spriteMgr->add_sprite(pixel_UV_size * 28.f, pixel_UV_size * 4.f, pixel_UV_size * 4.f, pixel_UV_size * 12.f); //right center
	spriteMgr->add_sprite(pixel_UV_size * 28.f, pixel_UV_size * 16.f, pixel_UV_size * 4.f, pixel_UV_size * 4.f); //right top

	//blocks
	for (int i = 0; i < 30; i++) {
		int yTex = i / 28;
		int xTex = i % 28;
		spriteMgr->add_sprite_bordered((float)xTex, (float)yTex, 1.0f, 1.0f);
		ObjectsDB::objectInfo[i + 2]->sprite_id = spriteMgr->get_last_index();
	}

	//oak tree top
	spriteMgr->add_sprite_bordered(5.f, 8.f, 3.f, 3.f);
	ObjectsDB::objectInfo[32]->sprite_id = spriteMgr->get_last_index();

	for (int i = 0; i < 6; i++) { //walls
		spriteMgr->add_sprite_bordered((float)i, 3.f, 2.f, 2.f);
		ObjectsDB::objectInfo[i + 33]->sprite_id = spriteMgr->get_last_index();
	}

	//complex objects texture coords
	//workbench
	spriteMgr->add_sprite_bordered(0.f, 5.f, 2.f, 1.f);
	ObjectsDB::objectInfo[39]->sprite_id = spriteMgr->get_last_index();
	
	//furnace
	spriteMgr->add_sprite_bordered(2.f, 5.f, 3.f, 2.f);
	ObjectsDB::objectInfo[40]->sprite_id = spriteMgr->get_last_index();
	
	//anvil
	spriteMgr->add_sprite_bordered(5.f, 5.f, 2.f, 1.f);
	ObjectsDB::objectInfo[41]->sprite_id = spriteMgr->get_last_index();
	
	//table
	spriteMgr->add_sprite_bordered(7.f, 5.f, 3.f, 2.f);
	ObjectsDB::objectInfo[42]->sprite_id = spriteMgr->get_last_index();
	
	//chair(to left)
	spriteMgr->add_sprite_bordered(10.f, 5.f, 1.f, 2.f);
	ObjectsDB::objectInfo[43]->sprite_id = spriteMgr->get_last_index();
	
	//chair(to right)
	spriteMgr->add_sprite_bordered(11.f, 5.f, 1.f, 2.f);
	ObjectsDB::objectInfo[44]->sprite_id = spriteMgr->get_last_index();
	
	//door(closed)
	spriteMgr->add_sprite_bordered(4.f, 8.f, 1.f, 3.f);
	ObjectsDB::objectInfo[45]->sprite_id = spriteMgr->get_last_index();
	
	//door(to left)
	spriteMgr->add_sprite_bordered(0.f, 8.f, 2.f, 3.f);
	ObjectsDB::objectInfo[46]->sprite_id = spriteMgr->get_last_index();
	
	//door(to right)
	spriteMgr->add_sprite_bordered(2.f, 8.f, 2.f, 3.f);
	ObjectsDB::objectInfo[47]->sprite_id = spriteMgr->get_last_index();
	
	//chest
	spriteMgr->add_sprite_bordered(12.f, 5.f, 2.f, 2.f);
	ObjectsDB::objectInfo[48]->sprite_id = spriteMgr->get_last_index();
	
	//life crystal
	spriteMgr->add_sprite_bordered(16.f, 5.f, 2.f, 2.f);
	ObjectsDB::objectInfo[81]->sprite_id = spriteMgr->get_last_index();
	
	//weapons, coins and materials
	for (int i = 0; i < 26; i++) {
		int yTex = i / 32;
		int xTex = i % 32;
		spriteMgr->add_sprite(block_UV_size * xTex, 1.0 - block_UV_size * (9 + yTex) - block_UV_size, block_UV_size, block_UV_size);
		ObjectsDB::objectInfo[i + 49]->sprite_id = spriteMgr->get_last_index();
	}

	//ammo
	for (int i = 0; i < 6; i++) {
		spriteMgr->add_sprite(
			block_UV_size * 26 + 0.6875 * block_UV_size * i + 0.03125 * block_UV_size,
			1.0 - block_UV_size * 9 - block_UV_size, block_UV_size, block_UV_size
		);
		ObjectsDB::objectInfo[i + 75]->sprite_id = spriteMgr->get_last_index();
	}

	for (int i = 0; i < 14; i++) { //items after ammo
		spriteMgr->add_sprite(block_UV_size * i, 1.0 - block_UV_size * 10.f - block_UV_size, block_UV_size, block_UV_size);
		ObjectsDB::objectInfo[i + 84]->sprite_id = spriteMgr->get_last_index();
	}

	//bullet texture
	spriteMgr->add_sprite(block_UV_size * 18.f, 1.0 - block_UV_size * 10.f - block_UV_size, block_UV_size, block_UV_size);
	ObjectsDB::objectInfo[98]->sprite_id = spriteMgr->get_last_index();
	
	//pistol texture
	spriteMgr->add_sprite(block_UV_size * 16.f, 1.0 - block_UV_size * 10.f - block_UV_size, block_UV_size * 2.f, block_UV_size);
	ObjectsDB::objectInfo[99]->sprite_id = spriteMgr->get_last_index();
	
	//shotgun texture
	spriteMgr->add_sprite(block_UV_size * 20.f, 1.0 - block_UV_size * 10 - block_UV_size, block_UV_size * 2.f, block_UV_size);
	ObjectsDB::objectInfo[100]->sprite_id = spriteMgr->get_last_index();
	
	/*
	//ambient objects
	ambientController = AmbientController(dayTime);
	//sun
	ambientController.sun.sprite_size = glm::vec2(4.f, 4.f);
	spriteMgr->add_sprite(10.f * block_UV_size, 23.f * block_UV_size, block_UV_size * 3.f, block_UV_size * 3.f);
	ambientController.sprites[0] = spriteMgr->get_last_index();
	//moon1
	ambientController.moon[0].sprite_size = glm::vec2(2.f, 2.f);
	spriteMgr->add_sprite(13.f * block_UV_size, 23.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 2.f);
	ambientController.sprites[1] = spriteMgr->get_last_index();
	//moon2
	ambientController.moon[1].sprite_size = glm::vec2(2.f, 2.f);
	spriteMgr->add_sprite(15.f * block_UV_size, 23.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 2.f);
	ambientController.sprites[2] = spriteMgr->get_last_index();
	//clouds and far clouds
	ambientController.clouds[0].position = { 0.f, ScreenHeight - ScreenWidth * 0.125f };
	ambientController.clouds[1].position = { ScreenWidth, ScreenHeight - ScreenWidth * 0.125f };
	ambientController.far_clouds[0].position = { 0.f, ScreenHeight - ScreenWidth * 0.13f };
	ambientController.far_clouds[1].position = { ScreenWidth * 0.5, ScreenHeight - ScreenWidth * 0.13f };
	ambientController.far_clouds[2].position = { ScreenWidth, ScreenHeight - ScreenWidth * 0.13f };
	spriteMgr->add_sprite(0.f, 12.f * block_UV_size, block_UV_size * 16.f, block_UV_size * 2.f);
	ambientController.sprites[3] = spriteMgr->get_last_index();
	//sky stars
	ambientController.sky_stars[0].position = { ScreenWidth * 0.5, 0.f };
	ambientController.sky_stars[1].position = { 0.f, ScreenHeight * 0.f };
	ambientController.sky_stars[2].position = { -ScreenWidth * 0.5, 0.f };
	ambientController.sky_stars[3].position = { ScreenWidth * 0.5, ScreenHeight * 0.5 };
	ambientController.sky_stars[4].position = { 0.f, ScreenHeight * 0.5f };
	ambientController.sky_stars[5].position = { -ScreenWidth * 0.5, ScreenHeight * 0.5 };
	spriteMgr->add_sprite(0.f, 3.f * block_UV_size, block_UV_size * 16.f, block_UV_size * 9.f);
	ambientController.sprites[4] = spriteMgr->get_last_index();
	*/

	//craftable items info
	craftable_items[0] = CraftableItem(38, 1, Nothing, std::vector<CraftingPair>{CraftingPair{ 6, 15 }}); //workbench
	craftable_items[1] = CraftableItem(28, 2, Nothing, std::vector<CraftingPair>{CraftingPair{ 6, 1 }}); //2 oak platforms
	craftable_items[2] = CraftableItem(22, 3, Nothing, std::vector<CraftingPair>{CraftingPair{ 6, 1 }, CraftingPair{ 73, 1 }}); //3 torches
	craftable_items[3] = CraftableItem(25, 3, Nothing, std::vector<CraftingPair>{CraftingPair{ 14, 1 }, CraftingPair{ 22, 3 }}); //3 ice torches
	craftable_items[4] = CraftableItem(39, 1, Workbench, std::vector<CraftingPair>{CraftingPair{ 1, 20 }, CraftingPair{ 22, 3 }, CraftingPair{ 6, 4 }}); //furnace
	craftable_items[5] = CraftableItem(40, 1, Workbench, std::vector<CraftingPair>{CraftingPair{ 71, 5 }}); //anvil

	craftable_items[6] = CraftableItem(70, 1, Furnace, std::vector<CraftingPair>{CraftingPair{ 9, 3 }}); //copper ingot
	craftable_items[7] = CraftableItem(71, 1, Furnace, std::vector<CraftingPair>{CraftingPair{ 10, 3 }}); //iron ingot
	craftable_items[8] = CraftableItem(72, 1, Furnace, std::vector<CraftingPair>{CraftingPair{ 11, 4 }}); //gold ingot

	craftable_items[9] = CraftableItem(74, 25, Workbench, std::vector<CraftingPair>{CraftingPair{ 6, 1 }, CraftingPair{ 1, 1 }}); //wooden arrow
	craftable_items[10] = CraftableItem(75, 10, Nothing, std::vector<CraftingPair>{CraftingPair{ 74, 10 }, CraftingPair{22, 1}}); //flaming arrow
	craftable_items[11] = CraftableItem(76, 10, Nothing, std::vector<CraftingPair>{CraftingPair{ 74, 10 }, CraftingPair{25, 1}}); //frostburn arrow

	//player info
	//standing pos
	spriteMgr->add_sprite(0.f, 17.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	player.sprites_ids[0] = spriteMgr->get_last_index();
	//jumping pos
	spriteMgr->add_sprite(2.f * block_UV_size, 17.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	player.sprites_ids[1] = spriteMgr->get_last_index();
	//walk 1
	spriteMgr->add_sprite(4.f * block_UV_size, 17.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	player.sprites_ids[2] = spriteMgr->get_last_index();
	//walk 2
	spriteMgr->add_sprite(6.f * block_UV_size, 17.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	player.sprites_ids[3] = spriteMgr->get_last_index();
	//walk 3
	spriteMgr->add_sprite(10.f * block_UV_size, 17.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	player.sprites_ids[4] = spriteMgr->get_last_index();
	//walk 4
	spriteMgr->add_sprite(12.f * block_UV_size, 17.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	player.sprites_ids[5] = spriteMgr->get_last_index();
	
	//entities info
	EntityDB::entityInfo[0] = std::make_unique<ProjectileInfo>(Arrow, 74, 5, 1, true, glm::vec2(1.f, 1.f));
	EntityDB::entityInfo[1] = std::make_unique<ProjectileInfo>(Arrow, 75, 7, 1, true, glm::vec2(1.f, 1.f), 1, 5);
	EntityDB::entityInfo[2] = std::make_unique<ProjectileInfo>(Arrow, 76, 9, 1, true, glm::vec2(1.f, 1.f), 2, 6);
	EntityDB::entityInfo[3] = std::make_unique<ProjectileInfo>(Arrow, 77, 10, 9999, false, glm::vec2(1.f, 1.f), 0, 7);
	EntityDB::entityInfo[4] = std::make_unique<ProjectileInfo>(Arrow, 78, 14, 1, true, glm::vec2(1.f, 1.f), 0, 8);
	EntityDB::entityInfo[5] = std::make_unique<ProjectileInfo>(Arrow, 79, 16, 1, true, glm::vec2(1.f, 1.f), 0, 9);

	EntityDB::entityInfo[11] = std::make_unique<ProjectileInfo>(MagicBall, -1, 9999999, 1, false, glm::vec2(2.f, 1.f), 0, 10); //NULL projectile
	EntityDB::entityInfo[12] = std::make_unique<ProjectileInfo>(Bullet, 97, 7, 1, false, glm::vec2(1.f, 1.f), 0, 11); //bullet projectile
	EntityDB::entityInfo[13] = std::make_unique<ProjectileInfo>(Throwable, 94, 10, 4, true, glm::vec2(1.f, 1.f)); //shuriken projectile
	EntityDB::entityInfo[14] = std::make_unique<ProjectileInfo>(Throwable, 95, 5, 2, true, glm::vec2(1.f, 1.f)); //throwable knife projectile

	uint32_t* sprites_ids_ptr;
	for (int i = 0; i < 6; i++) { //projectile arrows tex
		sprites_ids_ptr = EntityDB::entityInfo[i]->get_sprites_ids();
		sprites_ids_ptr[0] = ObjectsDB::objectInfo[i + 74]->sprite_id;
	}
	//"NULL" projectile
	sprites_ids_ptr = EntityDB::entityInfo[11]->get_sprites_ids();
	spriteMgr->add_sprite(14.f * block_UV_size, 1.0 - block_UV_size * 10 - block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	//bullet projectile
	sprites_ids_ptr = EntityDB::entityInfo[12]->get_sprites_ids();
	spriteMgr->add_sprite(19.f * block_UV_size, 1.0 - block_UV_size * 10 - block_UV_size, block_UV_size, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	//shuriken projectile
	sprites_ids_ptr = EntityDB::entityInfo[13]->get_sprites_ids();
	spriteMgr->add_sprite(11.f * block_UV_size, 1.0 - block_UV_size * 10 - block_UV_size, block_UV_size, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	//throwable knife
	sprites_ids_ptr = EntityDB::entityInfo[14]->get_sprites_ids();
	spriteMgr->add_sprite(12.f * block_UV_size, 1.0 - block_UV_size * 10 - block_UV_size, block_UV_size, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();

	//Slimes
	EntityDB::entityInfo[6] = std::make_unique<SlimeInfo>("Slime", 30, 5, 0, 12.f, 3.f, glm::vec2(1.375f, 1.18f), glm::vec2(2.f, 2.f));
	std::vector<DropInfo>& enemy_drop_info = EntityDB::entityInfo[6]->get_drop_v();
	enemy_drop_info.reserve(2);
	enemy_drop_info.emplace_back(DropInfo(66, 100.f, 5, 10));
	enemy_drop_info.emplace_back(DropInfo(73, 80.f, 1, 2));

	sprites_ids_ptr = EntityDB::entityInfo[6]->get_sprites_ids();
	spriteMgr->add_sprite(10.f * block_UV_size, 23.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 2.f);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	spriteMgr->add_sprite(12.f * block_UV_size, 23.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 2.f);
	sprites_ids_ptr[1] = spriteMgr->get_last_index();

	//Zombies
	EntityDB::entityInfo[7] = std::make_unique<ZombieInfo>("Zombie", 500, 10, 2, 12.f, 2.f, glm::vec2(1.5f, 2.85f), glm::vec2(2.f, 3.f));
	std::vector<DropInfo>& enemy_drop_info2 = EntityDB::entityInfo[7]->get_drop_v();
	enemy_drop_info2.reserve(2);
	enemy_drop_info2.emplace_back(DropInfo(66, 100, 20));
	enemy_drop_info2.emplace_back(DropInfo(67, 50, 1));

	sprites_ids_ptr = EntityDB::entityInfo[7]->get_sprites_ids();
	//standing pos
	spriteMgr->add_sprite(0.f, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	//jumping pos
	spriteMgr->add_sprite(2.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	sprites_ids_ptr[1] = spriteMgr->get_last_index();
	//walk 1
	spriteMgr->add_sprite(4.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	sprites_ids_ptr[2] = spriteMgr->get_last_index();
	//walk 2
	spriteMgr->add_sprite(6.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	sprites_ids_ptr[3] = spriteMgr->get_last_index();
	//walk 3
	spriteMgr->add_sprite(8.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	sprites_ids_ptr[4] = spriteMgr->get_last_index();
	//walk 4
	spriteMgr->add_sprite(10.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size * 3.f);
	sprites_ids_ptr[5] = spriteMgr->get_last_index();

	//Flying eyes
	EntityDB::entityInfo[8] = std::make_unique<FlyingEyeInfo>("Demon Eye", 60, 18, 2, 12.f, 3.f, glm::vec2(1.f, 1.f), glm::vec2(3.f, 1.5f));
	std::vector<DropInfo>& enemy_drop_info3 = EntityDB::entityInfo[8]->get_drop_v();
	enemy_drop_info3.reserve(2);
	enemy_drop_info3.emplace_back(DropInfo(66, 100.f, 75));
	enemy_drop_info3.emplace_back(DropInfo(73, 80.f, 1, 2));

	sprites_ids_ptr = EntityDB::entityInfo[8]->get_sprites_ids();
	spriteMgr->add_sprite(12.f * block_UV_size, 16.f * block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	spriteMgr->add_sprite(14.f * block_UV_size, 16.f * block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[1] = spriteMgr->get_last_index();

	EntityDB::entityInfo[9] = std::make_unique<FlyingEyeInfo>("Dilated Eye", 50, 17, 0, 12.f, 3.f, glm::vec2(1.f, 1.f), glm::vec2(3.f, 1.5f));
	std::vector<DropInfo>& enemy_drop_info4 = EntityDB::entityInfo[9]->get_drop_v();
	enemy_drop_info4.reserve(2);
	enemy_drop_info4.emplace_back(DropInfo(66, 100.f, 75));
	enemy_drop_info4.emplace_back(DropInfo(73, 80.f, 1, 2));

	sprites_ids_ptr = EntityDB::entityInfo[9]->get_sprites_ids();
	spriteMgr->add_sprite(12.f * block_UV_size, 15.f * block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	spriteMgr->add_sprite(14.f * block_UV_size, 15.f * block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[1] = spriteMgr->get_last_index();

	EntityDB::entityInfo[10] = std::make_unique<FlyingEyeInfo>("Green Eye", 60, 19, 0, 12.f, 3.f, glm::vec2(1.f, 1.f), glm::vec2(3.f, 1.5f));
	std::vector<DropInfo>& enemy_drop_info5 = EntityDB::entityInfo[10]->get_drop_v();
	enemy_drop_info5.reserve(2);
	enemy_drop_info5.emplace_back(DropInfo(66, 100.f, 75));
	enemy_drop_info5.emplace_back(DropInfo(73, 80.f, 1, 2));

	sprites_ids_ptr = EntityDB::entityInfo[10]->get_sprites_ids();
	spriteMgr->add_sprite(12.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[0] = spriteMgr->get_last_index();
	spriteMgr->add_sprite(14.f * block_UV_size, 14.f * block_UV_size, block_UV_size * 2.f, block_UV_size);
	sprites_ids_ptr[1] = spriteMgr->get_last_index();

	//effects info
	effectsManager = Effects::EffectsManager::get_instance();
	effectsManager->init(10);
	/*0*/  effectsManager->add_effect_info(Effects::EffectType::isImmediateBuff, "LifeCrystalBuff", Effects::StatName::isHP, 20.0f);
	/*1*/  effectsManager->add_effect_info(Effects::EffectType::isImmediateBuff, "ManaCrystalBuff", Effects::StatName::isMana, 20.0f);
	/*2*/  effectsManager->add_effect_info(Effects::EffectType::isBuff, "DefenceBuff", Effects::StatName::isDef, 10.0f);
	/*3*/  effectsManager->add_effect_info(Effects::EffectType::isBuff, "SpeedBuff", Effects::StatName::isSpeed, 0.25f);
	/*4*/  effectsManager->add_effect_info(Effects::EffectType::isBuff, "RegenBuff", Effects::StatName::isRegen, 0.5f);
	/*5*/  effectsManager->add_effect_info(Effects::EffectType::isDamagingDebuff, "Burning", Effects::StatName::isHP, 2.0f, 1, 0.1f, 0.25f);
	/*6*/  effectsManager->add_effect_info(Effects::EffectType::isDamagingDebuff, "FrostBurning", Effects::StatName::isHP, 2.0f, 2, 0.1f, 0.25f);
	/*7*/  effectsManager->add_effect_info(Effects::EffectType::isDamagingDebuff, "Poisoning", Effects::StatName::isHP, 1.0f, 0, 0.0f, 0.25f);
	/*8*/  effectsManager->add_effect_info(Effects::EffectType::isHealSickness, "PotionSickness", Effects::StatName::isHP, 0.0f);
	/*9*/  effectsManager->add_effect_info(Effects::EffectType::isHealing, "MushroomHeal", Effects::StatName::isHP, 25.0f);
	/*10*/ effectsManager->add_effect_info(Effects::EffectType::isHealing, "PotionHeal", Effects::StatName::isHP, 50.0f);

	for (int i = 0; i < 7; i++) { //effects sprites
		spriteMgr->add_sprite(block_UV_size * i, 1.0 - block_UV_size * 11 - block_UV_size, block_UV_size, block_UV_size);
		effectsManager->get_effect_info(i + 2).sprite_id = spriteMgr->get_last_index();
	}

	//Particles
	particlesMgr = ParticlesManager::get_instance();
	particlesSystem = ParticleSystem::get_instance();
	
	spriteMgr->add_sprite(14.f * block_UV_size, 1.0 - block_UV_size * 6 - block_UV_size, block_UV_size, block_UV_size);
	particlesMgr->add_particle_info((uint32_t)spriteMgr->get_last_index(), 12); //flame light

	spriteMgr->add_sprite(15.f * block_UV_size, 1.0 - block_UV_size * 6 - block_UV_size, block_UV_size, block_UV_size);
	particlesMgr->add_particle_info((uint32_t)spriteMgr->get_last_index(), 13); //frostburn light

	//reserve memory for objects
	
	dropped_items.reserve(1000);
	damage_text.reserve(100);

	player.inventory.init();
	for (int i = 2; i < 31; i++) {
		uint16_t amount = rand() % 10000;
		player.inventory.place_item(i, amount);
	}
	for (int i = 39; i < 49; i++) {
		uint16_t amount = rand() % 10;
		player.inventory.place_item(i, amount);
	}
	uint16_t amount = 1;
	player.inventory.place_item(49, amount); //copper pickaxe

	world = std::make_unique<World>();
	world->init(&player);

	ui_renderer = UI_Renderer::get_instance();
	ui_renderer->init(&player);
	ui_renderer->init_basic_inventory_slots_data();

	//audio manager

	//openGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::main_loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (!update())
			glfwSetWindowShouldClose(window, true);

		render();

		input_end_frame();
	}
}

void Game::init_open_gl() {
	glfwInit();
	//version of OpenGL (version 4.6)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//using the core profile for only modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1920, 1080, "Sandbox Game?", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetWindowAspectRatio(window, 16, 9);
	//glfwSwapInterval(0);

	gladLoadGL();

	//openGL version and GPU info
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	renderer = std::make_unique<OpenGL_Renderer>(window);
}

void Game::init_input() {
	InputHandler::setGLFWwindowCallbacks(window);
}

void Game::uninit() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
