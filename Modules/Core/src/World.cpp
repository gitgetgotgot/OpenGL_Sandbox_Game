#include <Core/World.h>
#include <Utility/TimeManager.h>

void World::init(Player* player) {
	main_player_ptr = player;
	//create world slots
	world_slots = new WorldSlot[width * height];
	MAX_CHUNK_X = width / CHUNK_SIZE;
	MAX_CHUNK_Y = height / CHUNK_SIZE;

	spriteMgr = SpriteManager::get_instance();

	//CONSTRAINTS
	const uint32_t MAX_SQUARES = CHUNK_SIZE * CHUNK_SIZE * 2;
	const uint32_t MAX_VERTEX_AMOUNT = MAX_SQUARES * 4;
	const uint32_t MAX_INDEX_AMOUNT = MAX_SQUARES * 6;

	shader = std::make_unique<ShaderProgram>("Resources/shaders/sprites.vert", "Resources/shaders/sprites.frag");

	ebo = std::make_unique<EBO>();
	GLuint* sprite_index_buffer = new GLuint[MAX_INDEX_AMOUNT];
	for (int i = 0; i < MAX_SQUARES; i++) {
		sprite_index_buffer[i * 6] = sprite_index_buffer[i * 6 + 3] = i * 4;
		sprite_index_buffer[i * 6 + 2] = sprite_index_buffer[i * 6 + 4] = sprite_index_buffer[i * 6] + 2;
		sprite_index_buffer[i * 6 + 1] = sprite_index_buffer[i * 6 + 2] - 1;
		sprite_index_buffer[i * 6 + 5] = sprite_index_buffer[i * 6 + 2] + 1;
	}
	ebo->set_data(sprite_index_buffer, MAX_INDEX_AMOUNT * sizeof(GLuint), GL_STATIC_DRAW);
	delete[] sprite_index_buffer;

	for (auto& buf : render_buffers) {
		buf = new ChunkRenderBuffer;
		buf->vertices.reserve(MAX_VERTEX_AMOUNT);
		buf->vao = std::make_unique<VAO>();
		buf->vao->bind_VAO();
		buf->vbo = std::make_unique<VBO>();
		buf->vbo->set_data(nullptr, MAX_VERTEX_AMOUNT * sizeof(Vertex2f), GL_DYNAMIC_DRAW);
		ebo->bind_EBO();
		buf->vao->link_Attribute(0, 2, GL_FLOAT, sizeof(Vertex2f), (void*)0);
		buf->vao->link_Attribute(1, 2, GL_FLOAT, sizeof(Vertex2f), (void*)(2 * sizeof(float)));
		buf->vao->unbind_VAO();
		buf->vbo->unbind_VBO();
		ebo->unbind_EBO();
	}

	sprites_ubo = std::make_unique<UBO>();
	sprites_ubo->bind_UBO(0);
	sprites_ubo->set_data(nullptr, sizeof(Universal_UBO_Data), GL_DYNAMIC_DRAW);

	//camera base options
	main_camera.zoom = MIN_ZOOM;
	main_camera.set_ortho_projection(-SystemContext::screen.ratio, SystemContext::screen.ratio, -1.0f, 1.0f);

	sprites_ubo_data.day_ratio = 1.0f; //1 for now;
	sprites_ubo_data.world_size = { width, height };
	sprites_ubo_data.viewMatrix = main_camera.viewMatrix;
	sprites_ubo_data.projectionMatrix = main_camera.projectionMatrix;

	sprites_ubo->update_data(&sprites_ubo_data, sizeof(Universal_UBO_Data));

	generate_world_data();

	spriteLightMapSSBO = std::make_unique<SSBO>();
	spriteLightMapSSBO->set_persistent_storage_data(nullptr, width * height * sizeof(ShaderLightingInfo));
	lightMap_data = static_cast<ShaderLightingInfo*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ShaderLightingInfo) * width * height, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
	
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			lightMap_data[j * width + i] = ShaderLightingInfo{ 0.f, 1.f, 0.f, 1.f, 0.5f };
		}
	}

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			load_chunk_buffer(x, y, x * 4 + y);
		}
	}
}

void World::update() {
	//update camera pos
	float v_multiplier = 1.0f;
	if (SystemContext::keyBoard.key_is_held(Key::KeyLeftShift))
		v_multiplier = 10.0f;
	if (SystemContext::keyBoard.key_is_held(Key::KeyW)) {
		main_camera.pos.y += speed * TimeManager::deltaTime * v_multiplier;
	}
	else if (SystemContext::keyBoard.key_is_held(Key::KeyS)) {
		main_camera.pos.y -= speed * TimeManager::deltaTime * v_multiplier;
	}
	if (SystemContext::keyBoard.key_is_held(Key::KeyA)) {
		main_camera.pos.x -= speed * TimeManager::deltaTime * v_multiplier;
	}
	else if (SystemContext::keyBoard.key_is_held(Key::KeyD)) {
		main_camera.pos.x += speed * TimeManager::deltaTime * v_multiplier;
	}
	//update camera zoom
	if (SystemContext::keyBoard.key_is_pressed(Key::KeyPageUp)) {
		main_camera.zoom += 0.2f;
		if (main_camera.zoom > MAX_ZOOM) main_camera.zoom = MAX_ZOOM;
		main_camera.set_ortho_projection(-SystemContext::screen.ratio, SystemContext::screen.ratio, -1.0f, 1.0f);
		sprites_ubo_data.projectionMatrix = main_camera.projectionMatrix;
		std::cout << "camera zoom = " << main_camera.zoom << std::endl;
	}
	else if (SystemContext::keyBoard.key_is_pressed(Key::KeyPageDown)) {
		main_camera.zoom -= 0.2f;
		if (main_camera.zoom < MIN_ZOOM) main_camera.zoom = MIN_ZOOM;
		main_camera.set_ortho_projection(-SystemContext::screen.ratio, SystemContext::screen.ratio, -1.0f, 1.0f);
		sprites_ubo_data.projectionMatrix = main_camera.projectionMatrix;
		std::cout << "camera zoom = " << main_camera.zoom << std::endl;
	}

	if (main_camera.pos.x < 0.0f) main_camera.pos.x = 0.0f;
	if (main_camera.pos.y < 0.0f) main_camera.pos.y = 0.0f;
	if (main_camera.pos.x > float(width - 1)) main_camera.pos.x = float(width - 1);
	if (main_camera.pos.y > float(height - 1)) main_camera.pos.y = float(height - 1);

	main_camera.update_camera();

	//update sprites ubo
	sprites_ubo_data.viewMatrix = main_camera.viewMatrix;
	sprites_ubo->update_data(&sprites_ubo_data, sizeof(Universal_UBO_Data));

	SystemContext::mouse.world_x_pos = int(SystemContext::mouse.ortho_x_pos * main_camera.zoom + main_camera.pos.x);
	SystemContext::mouse.world_y_pos = int(SystemContext::mouse.ortho_y_pos * main_camera.zoom + main_camera.pos.y);

	uint32_t camera_chunk_x = uint32_t(main_camera.pos.x) / CHUNK_SIZE;
	uint32_t camera_chunk_y = uint32_t(main_camera.pos.y) / CHUNK_SIZE;
	update_chunks(camera_chunk_x, camera_chunk_y);

	if (SystemContext::keyBoard.key_is_pressed(Key::KeyE))
		main_player_ptr->inventory.toggle_inventory();
	main_player_ptr->inventory.update();

	if (!SystemContext::mouse.overlapped_by_UI_layer) {
		if (SystemContext::mouse.lb_is_pressed())
			process_LB_click();
		if (SystemContext::mouse.rb_is_pressed())
			process_RB_click();
		if (SystemContext::mouse.wheel_offset != 0.0f)
			process_scrollwheel();
	}
}

void World::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	shader->use_shader();
	int central_chunk_buf_x = last_chunk_x - current_buffer_x_min;
	int central_chunk_buf_y = last_chunk_y - current_buffer_y_min;
	int render_min_x = central_chunk_buf_x - 1;
	if (render_min_x < 0) render_min_x = 0;
	int render_max_x = central_chunk_buf_x + 1;
	if (render_max_x > 3) render_max_x = 3;
	int render_min_y = central_chunk_buf_y - 1;
	if (render_min_y < 0) render_min_y = 0;
	int render_max_y = central_chunk_buf_y + 1;
	if (render_max_y > 3) render_max_y = 3;
	for (int x = render_min_x; x <= render_max_x; x++) {
		for (int y = render_min_y; y <= render_max_y; y++) {
			ChunkRenderBuffer& buf = *render_buffers[x * 4 + y];
			renderer->renderIndexedDataVAOSpecific(buf.vao, buf.index_count);
		}
	}
}

void World::update_chunks(uint32_t& camera_chunk_x, uint32_t& camera_chunk_y) {
	bool camera_chunk_x_pos_changed = camera_chunk_x != last_chunk_x;
	bool camera_chunk_y_pos_changed = camera_chunk_y != last_chunk_y;
	int move_x_dir = 0, move_y_dir = 0;

	if (camera_chunk_x_pos_changed) {
		if (camera_chunk_x > last_chunk_x && camera_chunk_x == current_buffer_x_max) { //right
			current_buffer_x_max++;
			if (current_buffer_x_max > MAX_CHUNK_X) {
				current_buffer_x_max = MAX_CHUNK_X;
			}
			else {
				//load chunks
				move_x_dir = 1;
				current_buffer_x_min++;
			}
		}
		else if (camera_chunk_x == current_buffer_x_min) { //left
			current_buffer_x_min--;
			if (current_buffer_x_min < 0) {
				current_buffer_x_min = 0;
			}
			else {
				//load chunks
				move_x_dir = -1;
				current_buffer_x_max--;
			}
		}
		last_chunk_x = camera_chunk_x;
	}
	if (camera_chunk_y_pos_changed) {
		if (camera_chunk_y > last_chunk_y && camera_chunk_y == current_buffer_y_max) { //top
			current_buffer_y_max++;
			if (current_buffer_y_max > MAX_CHUNK_Y) {
				current_buffer_y_max = MAX_CHUNK_Y;
			}
			else {
				//load chunks
				move_y_dir = 1;
				current_buffer_y_min++;
			}
		}
		else if (camera_chunk_y == current_buffer_y_min) { //bottom
			current_buffer_y_min--;
			if (current_buffer_y_min < 0) {
				current_buffer_y_min = 0;
			}
			else {
				//load chunks
				move_y_dir = -1;
				current_buffer_y_max--;
			}
		}
		last_chunk_y = camera_chunk_y;
	}
	if (camera_chunk_x_pos_changed || camera_chunk_y_pos_changed) {
		load_chunk_buffers(move_x_dir, move_y_dir);
	}
}

void World::load_chunk_buffer(uint32_t chunk_world_x, uint32_t chunk_world_y, uint32_t chunk_buffer_index) {
	float startX = CHUNK_SIZE * chunk_world_x;
	float startY = CHUNK_SIZE * chunk_world_y;
	float lastX = startX + CHUNK_SIZE;
	float lastY = startY + CHUNK_SIZE;

	ChunkRenderBuffer& buffer = *render_buffers[chunk_buffer_index];
	buffer.x = chunk_world_x;
	buffer.y = chunk_world_y;
	sparse_set<Vertex2f>& vertices = buffer.vertices;
	vertices.clear();
	WorldSlot* slot = nullptr;
	glm::vec2* uv_ptr = nullptr;
	uint32_t base_index = 0;

	for (int x = startX; x < lastX; x++) {
		for (int y = startY; y < lastY; y++) {
			slot = &world_slots[y * width + x];
			if (slot->tile_id == 0) {
				base_index += 4;
				continue;
			}
			//add tile sprite data
			std::unique_ptr<ObjectInfo>& object_info = ObjectsDB::objectInfo[slot->tile_id];
			uv_ptr = spriteMgr->get_sprite(object_info->sprite_id).UV;
			
			if (object_info->objectType == ObjectType::isBlock) {
				vertices.add(Vertex2f{ glm::vec2(x, y), uv_ptr[0] }, base_index);
				vertices.add(Vertex2f{ glm::vec2(x, y + 1), uv_ptr[1] }, base_index + 1);
				vertices.add(Vertex2f{ glm::vec2(x + 1, y + 1), uv_ptr[2] }, base_index + 2);
				vertices.add(Vertex2f{ glm::vec2(x + 1, y), uv_ptr[3] }, base_index + 3);
			}
			else if (object_info->objectType == ObjectType::isComplexObject) {
				float x_size = object_info->get_sizeX();
				float y_size = object_info->get_sizeY();
				vertices.add(Vertex2f{ glm::vec2(x, y), uv_ptr[0] }, base_index);
				vertices.add(Vertex2f{ glm::vec2(x, y + y_size), uv_ptr[1] }, base_index + 1);
				vertices.add(Vertex2f{ glm::vec2(x + x_size, y + y_size), uv_ptr[2] }, base_index + 2);
				vertices.add(Vertex2f{ glm::vec2(x + x_size, y), uv_ptr[3] }, base_index + 3);
			}
			base_index += 4;
		}
	}
	buffer.vbo->update_data(vertices.data(), sizeof(Vertex2f) * vertices.size());
	buffer.index_count = (vertices.size() / 4) * 6;
}

void World::load_chunk_buffers(int x_move, int y_move) {
	
	auto move_ptrs_to_right = [&]() {
		for (int y = 0; y < 4; y++) {
			ChunkRenderBuffer* first_buf = render_buffers[y];
			for (int x = 0; x < 3; x++)
				render_buffers[x * 4 + y] = render_buffers[(x + 1) * 4 + y];
			render_buffers[12 + y] = first_buf;
		}
		};
	auto move_ptrs_to_left = [&]() {
		for (int y = 0; y < 4; y++) {
			ChunkRenderBuffer* last_buf = render_buffers[12 + y];
			for (int x = 3; x > 0; x--)
				render_buffers[x * 4 + y] = render_buffers[(x - 1) * 4 + y];
			render_buffers[y] = last_buf;
		}
		};
	auto move_ptrs_to_top = [&]() {
		for (int x = 0; x < 4; x++) {
			ChunkRenderBuffer* first_buf = render_buffers[x * 4];
			for (int y = 0; y < 3; y++)
				render_buffers[x * 4 + y] = render_buffers[x * 4 + y + 1];
			render_buffers[x * 4 + 3] = first_buf;
		}
		};
	auto move_ptrs_to_bottom = [&]() {
		for (int x = 0; x < 4; x++) {
			ChunkRenderBuffer* last_buf = render_buffers[x * 4 + 3];
			for (int y = 3; y > 0; y--)
				render_buffers[x * 4 + y] = render_buffers[x * 4 + y - 1];
			render_buffers[x * 4] = last_buf;
		}
		};

	int chunks_column_offset, chunks_row_offset;
	int chunks_column_x, chunks_row_y;
	int rows_start_x = 0, rows_end_x = 4;
	if (x_move > 0) {
		move_ptrs_to_right();
		chunks_column_offset = 12;
		chunks_column_x = current_buffer_x_max;
		rows_end_x = 3;
	}
	else if (x_move < 0) {
		move_ptrs_to_left();
		chunks_column_offset = 0;
		chunks_column_x = current_buffer_x_min;
		rows_start_x = 1;
	}
	if (y_move > 0) {
		move_ptrs_to_top();
		chunks_row_offset = 3;
		chunks_row_y = current_buffer_y_max;
	}
	else if (y_move < 0) {
		move_ptrs_to_bottom();
		chunks_row_offset = 0;
		chunks_row_y = current_buffer_y_min;
	}
	if (x_move != 0) {
		int chunk_y = current_buffer_y_min;
		for (int y = 0; y < 4; y++) {
			load_chunk_buffer(chunks_column_x, chunk_y, chunks_column_offset + y);
			chunk_y++;
		}
	}
	if (y_move != 0) {
		int chunk_x = current_buffer_x_min + rows_start_x;
		for (int x = rows_start_x; x < rows_end_x; x++) {
			load_chunk_buffer(chunk_x, chunks_row_y, x * 4 + chunks_row_offset);
			chunk_x++;
		}
	}
}

void World::process_LB_click() {
	uint16_t active_item_id = main_player_ptr->inventory.get_active_item_id();
	if (active_item_id == 0) return; //do nothing if player is holding nothing

	ObjectInfo& active_item_info = *ObjectsDB::objectInfo[active_item_id];
	switch (active_item_info.objectType) {
	case ObjectType::isBlock: {
		place_tile(SystemContext::mouse.world_x_pos, SystemContext::mouse.world_y_pos, active_item_id);
		break;
	}
	case ObjectType::isComplexObject: {
		place_tile(SystemContext::mouse.world_x_pos, SystemContext::mouse.world_y_pos, active_item_id);
		break;
	}
	case ObjectType::isWeapon: {
		WeaponType weapon_type = active_item_info.get_weapon_type();
		if (weapon_type == WeaponType::isPickaxe) {
			destroy_tile(SystemContext::mouse.world_x_pos, SystemContext::mouse.world_y_pos);
		}
		break;
	}
	case ObjectType::isConsumable: {

		break;
	}
	case ObjectType::isPotion: {

		break;
	}
	}
}

void World::process_RB_click() {
	uint32_t slot_index = SystemContext::mouse.world_y_pos * width + SystemContext::mouse.world_x_pos;
	WorldSlot* slot = &world_slots[slot_index];

	if (slot->tile_id == 0) return; //no tile -> no interaction

	if (slot->tile_id == 1) { //complex object part -> get main part
		auto it = object_components.find(slot_index);
		if (it == object_components.end()) return; //this shouldn't happen, but ok
		uint32_t main_part_x = it->second->get_column();
		uint32_t main_part_y = it->second->get_line();
		slot_index = main_part_y * width + main_part_x; //get main slot index
		slot = &world_slots[slot_index]; //slot ptr now points on the main object part
	}

	ObjectInfo& slot_info = *ObjectsDB::objectInfo[slot->tile_id];
	if (slot_info.objectType == ObjectType::isComplexObject) { //complex object -> can be interactable
		ComplexObjectType type = slot_info.get_comp_obj_type();
		if (type == ComplexObjectType::isChest) {
			std::cout << "Interacted with chest" << std::endl;
			main_player_ptr->inventory.open_chest(object_components[slot_index]->get_chest_slots());
		}
		else if (type == ComplexObjectType::isDoor) {
			std::cout << "Interacted with door" << std::endl;
		}
	}
}

void World::process_scrollwheel() {

}

bool World::place_tile(uint32_t world_x, uint32_t world_y, uint16_t tile_id) {
	WorldSlot& slot = world_slots[world_y * width + world_x];
	if (slot.tile_id != 0) return false; //slot already has tile
	std::unique_ptr<ObjectInfo>& object_info = ObjectsDB::objectInfo[tile_id];
	if (object_info->objectType == ObjectType::isComplexObject) {
		int obj_width = object_info->get_sizeX();
		int obj_height = object_info->get_sizeY();
		int max_x = world_x + obj_width;
		int max_y = world_y + obj_height;
		for (int x = world_x; x < max_x; x++) {
			for (int y = world_y; y < max_y; y++) {
				if (world_slots[y * width + x].tile_id != 0) {
					return false; //not enough space to place complex object
				}
			}
		}
		int main_part_index = world_y * width + world_x;
		for (int x = world_x; x < max_x; x++) {
			for (int y = world_y; y < max_y; y++) {
				int comp_obj_part_index = y * width + x;
				if (comp_obj_part_index == main_part_index) continue;
				world_slots[y * width + x].tile_id = 1; //place complex object parts with coords of main part
				object_components.emplace(comp_obj_part_index, std::make_unique<ComplexObjectPartComponent>(world_x, world_y));
			}
		}
		ComplexObjectType type = object_info->get_comp_obj_type();
		if (type == ComplexObjectType::isChest) {
			object_components.emplace(main_part_index, std::make_unique<ChestComponent>());
		}
		else if (type == ComplexObjectType::isDoor) {
			object_components.emplace(main_part_index, std::make_unique<DoorComponent>(0));
		}
	}
	slot.tile_id = tile_id;

	//update chunk buffer if placed tile is in visible chunks buffer
	uint32_t chunk_x = uint32_t(world_x / CHUNK_SIZE);
	uint32_t chunk_y = uint32_t(world_y / CHUNK_SIZE);
	if (chunk_x >= current_buffer_x_min && chunk_x <= current_buffer_x_max &&
		chunk_y >= current_buffer_y_min && chunk_y <= current_buffer_y_max
		) {
		uint32_t chunk_buf_x = chunk_x - current_buffer_x_min;
		uint32_t chunk_buf_y = chunk_y - current_buffer_y_min;
		ChunkRenderBuffer& buffer = *render_buffers[chunk_buf_x * 4 + chunk_buf_y];
		sparse_set<Vertex2f>& vertices = buffer.vertices;

		uint32_t tile_chunk_x = world_x - chunk_x * CHUNK_SIZE;
		uint32_t tile_chunk_y = world_y - chunk_y * CHUNK_SIZE;
		uint32_t tile_index = (tile_chunk_x * CHUNK_SIZE + tile_chunk_y) * 4;

		glm::vec2* uv_ptr = spriteMgr->get_sprite(object_info->sprite_id).UV;

		if (object_info->objectType == ObjectType::isBlock) {
			vertices.add(Vertex2f{ glm::vec2(world_x, world_y), uv_ptr[0] }, tile_index);
			vertices.add(Vertex2f{ glm::vec2(world_x, world_y + 1), uv_ptr[1] }, tile_index + 1);
			vertices.add(Vertex2f{ glm::vec2(world_x + 1, world_y + 1), uv_ptr[2] }, tile_index + 2);
			vertices.add(Vertex2f{ glm::vec2(world_x + 1, world_y), uv_ptr[3] }, tile_index + 3);
		}
		else if (object_info->objectType == ObjectType::isComplexObject) {
			float x_size = object_info->get_sizeX();
			float y_size = object_info->get_sizeY();
			vertices.add(Vertex2f{ glm::vec2(world_x, world_y), uv_ptr[0] }, tile_index);
			vertices.add(Vertex2f{ glm::vec2(world_x, world_y + y_size), uv_ptr[1] }, tile_index + 1);
			vertices.add(Vertex2f{ glm::vec2(world_x + x_size, world_y + y_size), uv_ptr[2] }, tile_index + 2);
			vertices.add(Vertex2f{ glm::vec2(world_x + x_size, world_y), uv_ptr[3] }, tile_index + 3);
		}
		buffer.vbo->update_data(vertices.data(), sizeof(Vertex2f) * vertices.size());
		buffer.index_count += 6;
	}

	return true;
}

bool World::destroy_tile(uint32_t world_x, uint32_t world_y) {
	uint32_t slot_index = world_y * width + world_x;
	WorldSlot* slot = &world_slots[slot_index];
	if (slot->tile_id == 0) return false; //slot already has no tile
	
	if (slot->tile_id == 1) { //complex object part -> get main part
		std::cout << "Trying to destroy complex object part..." << std::endl;
		auto it = object_components.find(slot_index);
		if (it == object_components.end()) return false; //this shouldn't happen, but ok
		world_x = it->second->get_column();
		world_y = it->second->get_line();
		slot_index = world_y * width + world_x; //get main slot index
		slot = &world_slots[slot_index]; //slot ptr now points on the main object part
	}

	ObjectInfo& slot_info = *ObjectsDB::objectInfo[slot->tile_id];
	if (slot_info.objectType == ObjectType::isComplexObject) {
		int obj_width = slot_info.get_sizeX();
		int obj_height = slot_info.get_sizeY();
		int max_x = world_x + obj_width;
		int max_y = world_y + obj_height;
		uint32_t index;
		for (int x = world_x; x < max_x; x++) {
			for (int y = world_y; y < max_y; y++) {
				index = y * width + x;
				world_slots[index].tile_id = 0;
				object_components.erase(index); //remove all object parts and main part component (if exists)
			}
		}

	}
	slot->tile_id = 0;

	uint32_t chunk_x = uint32_t(world_x / CHUNK_SIZE);
	uint32_t chunk_y = uint32_t(world_y / CHUNK_SIZE);
	if (chunk_x >= current_buffer_x_min && chunk_x <= current_buffer_x_max &&
		chunk_y >= current_buffer_y_min && chunk_y <= current_buffer_y_max
		) {
		uint32_t chunk_buf_x = chunk_x - current_buffer_x_min;
		uint32_t chunk_buf_y = chunk_y - current_buffer_y_min;
		ChunkRenderBuffer& buffer = *render_buffers[chunk_buf_x * 4 + chunk_buf_y];
		sparse_set<Vertex2f>& vertices = buffer.vertices;

		uint32_t tile_chunk_x = world_x - chunk_x * CHUNK_SIZE;
		uint32_t tile_chunk_y = world_y - chunk_y * CHUNK_SIZE;
		uint32_t tile_index = (tile_chunk_x * CHUNK_SIZE + tile_chunk_y) * 4;

		vertices.remove(tile_index);
		vertices.remove(tile_index + 1);
		vertices.remove(tile_index + 2);
		vertices.remove(tile_index + 3);
		buffer.vbo->update_data(vertices.data(), sizeof(Vertex2f) * vertices.size());
		buffer.index_count -= 6;
	}
	return true;
}

void World::generate_world_data() {
	int biome_width = width / 10;
	//generate flat surface with tiny waves
	for (int i = 0; i < width; i++) {
		int j = 0;
		//int point = 891 + sin(0.1 * i) * 20;
		int point = 899 + sin(i * 0.05) * 1;
		for (; j < point; j++) {  //stone layer
			world_slots[j * width + i].tile_id = 2;
		}
		while (j < point + 8) { //dirt layer
			world_slots[j * width + i].tile_id = 3;
			j++;
		}
		world_slots[j * width + i].tile_id = 4; //dirt with grass
	}
	//generate some kind of desert on either left or right side of the map
	bool desertIsOnLeft = rand() % 2;
	int left_biome_border, right_biome_border;
	if (desertIsOnLeft) {
		left_biome_border = width * 0.3 - 1;
		right_biome_border = left_biome_border + biome_width;
	}
	else {
		left_biome_border = width * 0.6 - 1;
		right_biome_border = left_biome_border + biome_width;
	}
	float value = 0.f;
	for (int i = left_biome_border; i < right_biome_border; i++) {
		int point = 904 + sin(3.14159 * value / 180.) * 15;
		for (int j = 600; j < point; j++) {
			world_slots[j * width + i].tile_id = 5;
		}
		value += 180 / 420.f;
	}
	//generate oceans on both sides
	left_biome_border = 0;
	right_biome_border = left_biome_border + width * 0.05;
	value = 180.f;
	for (int i = left_biome_border; i < right_biome_border - 20; i++) { //place sand in a curve line and fill with water blocks on left side
		int point = 829 + (cos(3.14159 * value / 180.) + 1) * 80;
		int j = point - 40;
		for (; j < point; j++) {  //place sand
			world_slots[j * width + i].tile_id = 5;
		}
		while (j <= 905) {  //place water
			world_slots[j * width + i].tile_id = 13;
			j++;
		}
		while (j <= 907) {  //remove redundant blocks
			if (world_slots[j * width + i].tile_id > 0) {
				if (world_slots[j * width + i].tile_id != 5) {
					world_slots[j * width + i].tile_id = 0;
				}
			}
			j++;
		}
		value += 90 / 190.f;
	}
	for (int i = right_biome_border - 20; i < right_biome_border; i++) { //make straigh place for 20 blocks after the ocean
		int point = 908;
		for (int j = point - 40; j < point; j++) {
			world_slots[j * width + i].tile_id = 5;
		}
	}
	right_biome_border = width - 1;
	left_biome_border = right_biome_border - width * 0.05;
	value = 90.f;
	for (int i = left_biome_border; i <= right_biome_border; i++) { //place sand in a curve line and fill with water blocks on right side
		int point = 829 + (cos(3.14159 * value / 180.) + 1) * 80;
		int j = point - 40;
		for (; j < point; j++) {  //place sand
			world_slots[j * width + i].tile_id = 5;
		}
		while (j <= 905) {  //place water
			world_slots[j * width + i].tile_id = 13;
			j++;
		}
		while (j <= 907) {  //remove redundant blocks
			if (world_slots[j * width + i].tile_id > 0) {
				if (world_slots[j * width + i].tile_id != 5) {
					world_slots[j * width + i].tile_id = 0;
				}
			}
			j++;
		}
		value += 90 / 190.f;
	}
	for (int i = left_biome_border - 20; i < left_biome_border; i++) { //make straigh place for 20 blocks after the ocean
		int point = 908;
		for (int j = point - 40; j < point; j++) {
			world_slots[j * width + i].tile_id = 5;
		}
	}
}

void World::save_world_data_in_file(const char* filePath) {
	std::ofstream write(filePath, std::ios::binary);
	uint16_t write_value = 0;

	write.write(reinterpret_cast<const char*>(&width), sizeof(uint16_t));
	write.write(reinterpret_cast<const char*>(&height), sizeof(uint16_t));
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			uint32_t index = j * width + i;
			WorldSlot& slot = world_slots[index];
			ObjectInfo* obj_info = ObjectsDB::objectInfo[slot.tile_id].get();
			//write tile id
			write.write(reinterpret_cast<const char*>(&slot.tile_id), sizeof(uint16_t));
			//write wall id
			write.write(reinterpret_cast<const char*>(&slot.wall_id), sizeof(uint8_t));
			//write flags
			write.write(reinterpret_cast<const char*>(&slot.flags), sizeof(uint8_t));

			if (obj_info->objectType == ObjectType::isCompObjPart) {
				ObjectComponent* obj_comp = object_components[index].get();
				write_value = obj_comp->get_column();
				write.write(reinterpret_cast<const char*>(&write_value), sizeof(uint16_t));
				write_value = obj_comp->get_line();
				write.write(reinterpret_cast<const char*>(&write_value), sizeof(uint16_t));
			}
			else if (obj_info->objectType == isComplexObject) {
				ObjectComponent* obj_comp = object_components[index].get();
				ComplexObjectType complexType = obj_info->get_comp_obj_type();
				if (complexType == isDoor) {
					uint8_t door_state = obj_comp->get_door_state();
					write.write(reinterpret_cast<const char*>(&door_state), sizeof(uint8_t));
				}
				else if (complexType == isChest) {
					InventorySlot* chest_slots_ptr = obj_comp->get_chest_slots();
					for (int c = 0; c < 40; c++) {
						write.write(reinterpret_cast<const char*>(&chest_slots_ptr[c].item_id), sizeof(uint16_t));
						write.write(reinterpret_cast<const char*>(&chest_slots_ptr[c].amount), sizeof(uint16_t));
					}
				}
			}
		}
	}
	write.close();
}

void World::load_world_data_from_file(const char* filePath) {
	/*std::ifstream read(filePath, std::ios::binary);
	ObjectType type;
	unsigned short value, id, column, line;
	uint8_t door_state;
	read.read(reinterpret_cast<char*>(&world_width), sizeof(unsigned short));
	read.read(reinterpret_cast<char*>(&world_height), sizeof(unsigned short));

	sprites_Array = new WorldSlot * [world_width] {nullptr};
	for (int i = 0; i < world_width; i++) {
		sprites_Array[i] = new WorldSlot[world_height];
	}

	for (int i = 0; i < world_width; i++) {
		for (int j = 0; j < world_height; j++) {
			read.read(reinterpret_cast<char*>(&sprites_Array[i][j].wall_id), sizeof(unsigned short));
			read.read(reinterpret_cast<char*>(&id), sizeof(unsigned short));
			read.read(reinterpret_cast<char*>(&type), sizeof(ObjectType));
			if (type == isCompObjPart) {
				read.read(reinterpret_cast<char*>(&column), sizeof(unsigned short));
				read.read(reinterpret_cast<char*>(&line), sizeof(unsigned short));
				sprites_Array[i][j].object = GameObject(type, id, new ComplexObjectPartComponent(column, line));
			}
			else if (type == isComplexObject) {
				ComplexObjectType complexType = objectInfo[sprites_Array[i][j].object.object_id]->get_comp_obj_type();
				if (complexType == isDoor) {
					read.read(reinterpret_cast<char*>(&door_state), sizeof(uint8_t));
					sprites_Array[i][j].object = GameObject(type, id, new DoorComponent(door_state));
				}
				else if (complexType == isChest) {
					sprites_Array[i][j].object = GameObject(type, id, new ChestComponent);
					InventoryPair* chest_ptr = sprites_Array[i][j].object.component->get_chest_slots();
					for (int c = 0; c < 40; c++) {
						read.read(reinterpret_cast<char*>(&chest_ptr[c].amount), sizeof(unsigned short));
						read.read(reinterpret_cast<char*>(&chest_ptr[c].object.id), sizeof(unsigned short));
						read.read(reinterpret_cast<char*>(&chest_ptr[c].object.type), sizeof(ObjectType));
					}
				}
			}
			else {
				sprites_Array[i][j].object = GameObject(type, id);
			}
		}
	}
	read.close();*/
}

World::~World() {
	delete[] world_slots;
	for (int i = 0; i < RENDERABLE_CHUNKS_SIZE; i++) {
		delete render_buffers[i];
	}

	spriteLightMapSSBO->bind_SSBO();
	if (lightMap_data)
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	lightMap_data = nullptr;

	main_player_ptr = nullptr;
}
