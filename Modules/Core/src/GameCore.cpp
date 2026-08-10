#include <Core/GameCore.h>
#include <Entities/Projectile.h>
#include <Entities/Enemies/Slime.h>
#include <Entities/Enemies/Zombie.h>
#include <Entities/Enemies/FlyingEye.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <glm/gtx/string_cast.hpp>
#include <json/json.hpp>

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
			player.flip_x = true;
			if (keyStates[GLFW_KEY_LEFT_SHIFT])
				playerXinc = -player.speed * 5;
			else
				playerXinc = -player.speed;
		}
		if (keyStates[GLFW_KEY_D]) {
			player.flip_x = false;
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
				player.time_falling += deltaTime;
				//calculate distance with this time_falling between last and current distances and add to current player Y
				float distance = player.jump_V0 * player.time_falling - BLOCK_VISIBLE_SIZE * 2 * 9.8f * player.time_falling * player.time_falling / 2;
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
								player.time_falling = 0.f;
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
								player.time_falling = 0.f;
								player.hitbox.center.y = j * BLOCK_VISIBLE_SIZE - player.hitbox.size.y * 0.5f;
								player.fallingDistance = 0.f;
								player.jump_V0 = 0.f;
								break;
							case BOTTOM:
								if (playerYinc < 0.f) playerYinc = 0.f;
								has_bottom = true;
								player.time_falling = 0.f;
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
			player.time_falling = 0.f;
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
			if (player.flip_x) {
				entity_buf->tex_UV[0] = player.tex_UV[offset + 3];
				entity_buf->tex_UV[1] = player.tex_UV[offset + 2];
				entity_buf->tex_UV[2] = player.tex_UV[offset + 1];
				entity_buf->tex_UV[3] = player.tex_UV[offset];
				entity_buf->tex_id = 0.f;
			}
			else {
				entity_buf->tex_UV[0] = player.tex_UV[offset];
				entity_buf->tex_UV[1] = player.tex_UV[offset + 1];
				entity_buf->tex_UV[2] = player.tex_UV[offset + 2];
				entity_buf->tex_UV[3] = player.tex_UV[offset + 3];
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
					physx.time_falling += deltaTime;
					//calculate distance with this time_falling between last and current distances and add to current player Y
					float distance = physx.current_jump_V * physx.time_falling - BLOCK_VISIBLE_SIZE * 2 * 9.8f * physx.time_falling * physx.time_falling / 2;
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
			if (entity->flip_x) {
				entity_buf->tex_UV[0] = tex_coords_ptr[offset + 3];
				entity_buf->tex_UV[1] = tex_coords_ptr[offset + 2];
				entity_buf->tex_UV[2] = tex_coords_ptr[offset + 1];
				entity_buf->tex_UV[3] = tex_coords_ptr[offset];
				entity_buf->tex_id = 0.f;
			}
			else {
				entity_buf->tex_UV[0] = tex_coords_ptr[offset];
				entity_buf->tex_UV[1] = tex_coords_ptr[offset + 1];
				entity_buf->tex_UV[2] = tex_coords_ptr[offset + 2];
				entity_buf->tex_UV[3] = tex_coords_ptr[offset + 3];
				entity_buf->tex_id = 0.f;
			}
			entity_buf++;
			entities_count++;

			//entity effects
			MobStats& stats = entity->get_entity_stats();
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
						MobStats& stats = entity->get_entity_stats();
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
			if (projectile->flip_x) {
				entity_buf->tex_UV[0] = tex_coords_ptr[3];
				entity_buf->tex_UV[1] = tex_coords_ptr[2];
				entity_buf->tex_UV[2] = tex_coords_ptr[1];
				entity_buf->tex_UV[3] = tex_coords_ptr[0];
				entity_buf->tex_id = 0.f;
			}
			else {
				entity_buf->tex_UV[0] = tex_coords_ptr[0];
				entity_buf->tex_UV[1] = tex_coords_ptr[1];
				entity_buf->tex_UV[2] = tex_coords_ptr[2];
				entity_buf->tex_UV[3] = tex_coords_ptr[3];
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
				entity_buf->tex_UV[0] = active_weapon.tex_UV[1];
				entity_buf->tex_UV[1] = active_weapon.tex_UV[0];
				entity_buf->tex_UV[2] = active_weapon.tex_UV[3];
				entity_buf->tex_UV[3] = active_weapon.tex_UV[2];
			}
			else {
				entity_buf->tex_UV[0] = active_weapon.tex_UV[0];
				entity_buf->tex_UV[1] = active_weapon.tex_UV[1];
				entity_buf->tex_UV[2] = active_weapon.tex_UV[2];
				entity_buf->tex_UV[3] = active_weapon.tex_UV[3];
			}
			entity_buf->tex_id = 0.f;
			entity_buf++;
			entities_count++;
		}
		//update particles
		
		//add particles to entities buffer
		int size = particles_v.size();
		for (int i = 0; i < size; i++) {
			tex_coords_ptr = particlesInfo[particles_v[i].id].tex_UV;
			entity_buf->modelMatrix = particles_v[i].modelMatrix;
			entity_buf->tex_UV[0] = tex_coords_ptr[0];
			entity_buf->tex_UV[1] = tex_coords_ptr[1];
			entity_buf->tex_UV[2] = tex_coords_ptr[2];
			entity_buf->tex_UV[3] = tex_coords_ptr[3];
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
				item.time_falling += deltaTime;
				float distance = - BLOCK_VISIBLE_SIZE * 2 * 9.8f * item.time_falling * item.time_falling / 2;
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
								item.time_falling = 0.f;
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
								item.time_falling = 0.f;
								item.hitbox.center.y = j * BLOCK_VISIBLE_SIZE - item.hitbox.size.y * 0.5f;
								item.fallingDistance = 0.f;
								break;
							case BOTTOM:
								item.time_falling = 0.f;
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
	compsMgr = ComponentsManager::get_instance();

	ObjectsDB::objectInfo.reserve(120);
	ObjectsDB::objectInfo.resize(120);
	EntityDB::entityInfo.reserve(20);
	EntityDB::entityInfo.resize(20);
	
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

	load_sprites_JSON();
	load_items_data_JSON();
	load_entities_data_JSON();
	load_animations_data_JSON();

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

	//Particles
	particlesMgr = ParticlesManager::get_instance();
	particlesSystem = ParticleSystem::get_instance();
	
	spriteMgr->add_sprite_absolute(14.f * block_UV_size, 1.0 - block_UV_size * 6 - block_UV_size, block_UV_size, block_UV_size, 1.0f);
	particlesMgr->add_particle_info((uint32_t)spriteMgr->get_last_index(), 12); //flame light

	spriteMgr->add_sprite_absolute(15.f * block_UV_size, 1.0 - block_UV_size * 6 - block_UV_size, block_UV_size, block_UV_size, 1.0f);
	particlesMgr->add_particle_info((uint32_t)spriteMgr->get_last_index(), 13); //frostburn light

	//Crafting info
	craft_sys = CraftingSystem::get_instance();
	craft_sys->add(39, 1, CraftCondition::c_NOTHING, std::vector<CraftingPair>{CraftingPair{ 7, 15 }}); //workbench
	craft_sys->add(29, 2, CraftCondition::c_NOTHING, std::vector<CraftingPair>{CraftingPair{ 7, 1 }}); //2 oak platforms
	craft_sys->add(23, 3, CraftCondition::c_NOTHING, std::vector<CraftingPair>{CraftingPair{ 7, 1 }, CraftingPair{ 74, 1 }}); //3 torches
	craft_sys->add(26, 3, CraftCondition::c_NOTHING, std::vector<CraftingPair>{CraftingPair{ 15, 1 }, CraftingPair{ 23, 3 }}); //3 ice torches
	craft_sys->add(40, 1, CraftCondition::c_WORKBENCH, std::vector<CraftingPair>{CraftingPair{ 2, 20 }, CraftingPair{ 23, 3 }, CraftingPair{ 7, 4 }}); //furnace
	craft_sys->add(41, 1, CraftCondition::c_WORKBENCH, std::vector<CraftingPair>{CraftingPair{ 72, 5 }}); //anvil
	
	craft_sys->add(71, 1, CraftCondition::c_FURNACE, std::vector<CraftingPair>{CraftingPair{ 10, 3 }}); //copper ingot
	craft_sys->add(72, 1, CraftCondition::c_FURNACE, std::vector<CraftingPair>{CraftingPair{ 11, 3 }}); //iron ingot
	craft_sys->add(73, 1, CraftCondition::c_FURNACE, std::vector<CraftingPair>{CraftingPair{ 12, 4 }}); //gold ingot
	
	craft_sys->add(75, 25, CraftCondition::c_WORKBENCH, std::vector<CraftingPair>{CraftingPair{ 7, 1 }, CraftingPair{ 2, 1 }}); //wooden arrow
	craft_sys->add(76, 10, CraftCondition::c_NOTHING, std::vector<CraftingPair>{CraftingPair{ 75, 10 }, CraftingPair{ 23, 1 }}); //flaming arrow
	craft_sys->add(77, 10, CraftCondition::c_NOTHING, std::vector<CraftingPair>{CraftingPair{ 75, 10 }, CraftingPair{ 26, 1 }}); //frostburn arrow

	//reserve memory for objects
	dropped_items.reserve(1000);
	damage_text.reserve(100);

	player.inventory.init();
	for (int i = 2; i < 52; i++) {
		uint16_t amount = rand() % 10000;
		player.inventory.place_item(i, amount);
	}

	player.inventory.update_crafts();
	player.inventory.update_dynamic_craft_slots_items();
	player.inventory.current_player_flags = (uint32_t)CraftCondition::c_WORKBENCH;

	world = std::make_unique<World>();
	world->init(&player);

	ui_renderer = UI_Renderer::get_instance();
	ui_renderer->init(&player);
	ui_renderer->init_basic_inventory_slots_data();
	ui_renderer->init_icons_base_vertices();
	ui_renderer->update_items();

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

void Game::uninit() {
	glfwDestroyWindow(window);
	glfwTerminate();
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

void Game::load_sprites_JSON() {

	std::ifstream file("Resources/sprites.json");
	nlohmann::json json;
	file >> json;

	for (auto& item : json) {
		uint32_t padding = item["padding"];

		auto& sprite = item["sprite"];
		uint32_t u0 = sprite["U0"];
		uint32_t v0 = sprite["V0"];
		uint32_t w = sprite["w"];
		uint32_t h = sprite["h"];
		float base_size = sprite["base_size"];

		spriteMgr->add_sprite_pixels(u0, v0, w, h, base_size);
	}
}

void Game::load_items_data_JSON() {
	std::ifstream file("Resources/items.json");
	nlohmann::json json;
	file >> json;

	for (auto& item : json) {
		//basic common info
		uint32_t id = item["id"];
		uint32_t sprite_id = item["sprite"];
		std::string name = item["name"];
		uint32_t type_id = item["type"];
		uint32_t effect_id = 0, light_id = 0;

		//place object info
		if (type_id == 0) { //Base Object
			uint32_t sub_type_id = item["sub_type"];
			if (item.contains("effect_id")) effect_id = item["effect_id"];
			if (item.contains("light_id")) light_id = item["light_id"];
			ObjectsDB::objectInfo[id] = std::make_unique<ObjectInfo>((ObjectType)sub_type_id, name);
		}
		else if (type_id > 0 && type_id < 3) { //simple or complex
			uint32_t sub_type_id = item["sub_type"];
			float toughness = item["toughness"]; uint16_t drop_id = item["drop_id"];
			bool collision = false, platform_collision = false;
			if (item.contains("collision")) collision = item["collision"];
			if (item.contains("platform_collision")) platform_collision = item["platform_collision"];
			if (item.contains("effect_id")) effect_id = item["effect_id"];
			if (item.contains("light_id")) light_id = item["light_id"];

			if (type_id == 1) { //Simple Object
				ObjectsDB::objectInfo[id] = std::make_unique<BlockInfo>(
					name, (BlockType)sub_type_id, toughness, drop_id, collision, platform_collision, effect_id, light_id);
			}
			else { //Complex Object
				float width_blocks = item["width"], height_blocks = item["height"];
				ObjectsDB::objectInfo[id] = std::make_unique<ComplexObjectInfo>(
					name, (ComplexObjectType)sub_type_id, toughness, drop_id, width_blocks, height_blocks, collision, platform_collision, effect_id, light_id);
			}
		}
		else if (type_id == 3) { //Wall
			float toughness = item["toughness"]; uint16_t drop_id = item["drop_id"];
			ObjectsDB::objectInfo[id] = std::make_unique<WallInfo>(name, toughness, drop_id);
		}
		else if (type_id > 3 && type_id < 7) { //Weapon
			uint32_t sub_type_id = item["sub_type"];
			int damage = item["dmg"]; float speed = item["speed"];
			float size_x = item["size_x"], size_y = item["size_y"];
			float crit_chance = item["crit_ch"]; bool is_stackable = item["stackable"];
			uint32_t use_sound_id = item["use_sound_id"];
			uint32_t proj_id = 0; float proj_cd = 0.0f;
			if (item.contains("proj_id")) proj_id = item["proj_id"];
			if (item.contains("proj_cd")) proj_cd = item["proj_cd"];

			if (type_id == 4) { //Regular Weapon
				ObjectsDB::objectInfo[id] = std::make_unique<WeaponInfo>(
					name, (WeaponType)sub_type_id, damage, size_x, size_y, crit_chance, speed, is_stackable, use_sound_id, proj_id, proj_cd);
			}
			else if (type_id == 5) { //Instrumental Weapon
				float power = item["power"], range = item["range"];
				ObjectsDB::objectInfo[id] = std::make_unique<InstrumentalWeaponInfo>(
					name, (WeaponType)sub_type_id, power, range, damage, size_x, size_y, crit_chance, speed, is_stackable, use_sound_id, proj_id, proj_cd);
			}
			else { //Magical Weapon
				uint32_t mana_cost = item["mana_cost"];
				ObjectsDB::objectInfo[id] = std::make_unique<MagicalWeaponInfo>(
					name, (WeaponType)sub_type_id, mana_cost, damage, size_x, size_y, crit_chance, speed, is_stackable, use_sound_id, proj_id, proj_cd);
			}
		}
		else if (type_id == 7) { //Ammo
			uint32_t sub_type_id = item["sub_type"];
			int damage = item["dmg"];
			float size_x = item["size_x"], size_y = item["size_y"];
			uint32_t entity_id = item["entity_id"];
			if (item.contains("effect_id")) effect_id = item["effect_id"];
			if (item.contains("light_id")) light_id = item["light_id"];
			ObjectsDB::objectInfo[id] = std::make_unique<AmmoInfo>(name, (AmmoType)sub_type_id, damage, size_x, size_y, entity_id, effect_id, light_id);
		}

		ObjectsDB::objectInfo[id]->sprite_id = sprite_id;

		std::cout << "[Items] (id = " << id << "): " << name << ", Sprite id = " << sprite_id << std::endl;
	}
}

void Game::load_entities_data_JSON() {
	std::ifstream file("Resources/entities.json");
	nlohmann::json json;
	file >> json;

	for (auto& entity : json) {
		//basic common info
		uint32_t id = entity["id"];
		std::string name = entity["name"];
		uint32_t type_id = entity["type"];
		uint32_t effect_id = 0, light_id = 0;

		EntityType entity_type = (EntityType)type_id;
		if (entity_type == EntityType::isMob) {
			uint32_t mob_type_id = entity["mob_type"];
			uint32_t move_type_id = entity["move_type"];
			float HP = entity["HP"], DEF = entity["DEF"], DMG = entity["DMG"];
			float speed_x = entity["speed_x"], speed_y = entity["speed_y"];
			float sprite_w = entity["sprite_w"], sprite_h = entity["sprite_h"];
			float hitbox_w = entity["hitbox_w"], hitbox_h = entity["hitbox_h"];
			std::vector<nlohmann::json> drop = entity["drop"].get<std::vector<nlohmann::json>>();
			std::vector<DropInfo> drops;
			drops.reserve(drop.size());
			for (auto& d : drop) {
				uint32_t drop_id = d["item_id"];
				float chance = d["chance"];
				uint32_t min = d["min"], max = d["max"];
				drops.emplace_back(drop_id, chance, min, max);
			}
			uint32_t animator_id = entity["animator_id"];

			MobType mob_type = (MobType)mob_type_id;
			if (mob_type == MobType::isEnemy) {
				uint32_t enemy_type_id = entity["enemy_type"];
				EntityDB::entityInfo[id] = std::make_unique<EnemyInfo>(name, (EnemyType)enemy_type_id, (MovementType)move_type_id,
					HP, DMG, DEF, speed_x, speed_y, glm::vec2(hitbox_w, hitbox_h), glm::vec2(sprite_w, sprite_h), std::move(drops), animator_id);
			}
			else {
				EntityDB::entityInfo[id] = std::make_unique<MobInfo>(name, mob_type, (MovementType)move_type_id,
					HP, DMG, DEF, speed_x, speed_y, glm::vec2(hitbox_w, hitbox_h), glm::vec2(sprite_w, sprite_h), std::move(drops), animator_id);
			}
		}
	}
}

void Game::load_animations_data_JSON() {
	std::ifstream file("Resources/animations.json");
	nlohmann::json json;
	file >> json;

	for (auto& anim : json) {

	}
}
