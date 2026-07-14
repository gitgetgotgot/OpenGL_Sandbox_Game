#include <UI/UI_Renderer.h>
#include <Entities/Player.h>
#include <IOSystem/SystemContext.h>

void UI_Renderer::init(Player* player) {
	main_player_ptr = player;
	spriteMgr = SpriteManager::get_instance();
	sdf_font_manager.load_main_sdf_font("verdana_SDF");

	//CONSTRAINTS
	const uint32_t MAX_VERTEX_SIZE = MAX_UI_SPRITES_PER_DRAW * 4;
	const uint32_t MAX_INDEX_SIZE = MAX_UI_SPRITES_PER_DRAW * 6;

	ui_shader = std::make_unique<ShaderProgram>("Resources/shaders/ui_sprites.vert", "Resources/shaders/ui_sprites.frag");

	ebo = std::make_unique<EBO>();
	GLuint* sprite_index_buffer = new GLuint[MAX_INDEX_SIZE];
	for (int i = 0; i < MAX_UI_SPRITES_PER_DRAW; i++) {
		sprite_index_buffer[i * 6] = sprite_index_buffer[i * 6 + 3] = i * 4;
		sprite_index_buffer[i * 6 + 2] = sprite_index_buffer[i * 6 + 4] = sprite_index_buffer[i * 6] + 2;
		sprite_index_buffer[i * 6 + 1] = sprite_index_buffer[i * 6 + 2] - 1;
		sprite_index_buffer[i * 6 + 5] = sprite_index_buffer[i * 6 + 2] + 1;
	}
	ebo->set_data(sprite_index_buffer, MAX_INDEX_SIZE * sizeof(GLuint), GL_STATIC_DRAW);
	delete[] sprite_index_buffer;

	//slot buffers
	slots_vao = std::make_unique<VAO>();
	slots_vao->bind_VAO();
	slots_vbo = std::make_unique<VBO>();
	slots_vbo->set_data(nullptr, MAX_VERTEX_SIZE * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	slots_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	slots_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	slots_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	slots_vao->unbind_VAO();
	slots_vbo->unbind_VBO();
	ebo->unbind_EBO();

	//item buffers
	items_vao = std::make_unique<VAO>();
	items_vao->bind_VAO();
	items_vbo = std::make_unique<VBO>();
	items_vbo->set_data(nullptr, MAX_VERTEX_SIZE * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	items_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	items_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	items_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	items_vao->unbind_VAO();
	items_vbo->unbind_VBO();
	ebo->unbind_EBO();

	//sdf text shader and buffers
	sdf_text_shader = std::make_unique<ShaderProgram>("Resources/shaders/sdf_text.vert", "Resources/shaders/sdf_text.frag");
	text_vao = std::make_unique<VAO>();
	text_vao->bind_VAO();
	text_vbo = std::make_unique<VBO>();
	text_vbo->set_data(nullptr, MAX_VERTEX_SIZE * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	text_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	text_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	text_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	text_vao->unbind_VAO();
	text_vbo->unbind_VBO();
	ebo->unbind_EBO();

	tooltip_text_vao = std::make_unique<VAO>();
	tooltip_text_vao->bind_VAO();
	tooltip_text_vbo = std::make_unique<VBO>();
	tooltip_text_vbo->set_data(nullptr, MAX_TOOLTIP_TEXT_SIZE * 4 * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	tooltip_text_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	tooltip_text_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	tooltip_text_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	tooltip_text_vao->unbind_VAO();
	tooltip_text_vbo->unbind_VBO();
	ebo->unbind_EBO();

	ui_ubo = std::make_unique<UBO>();
	ui_ubo->bind_UBO(1);
	ubo_data.viewMatrix = glm::mat4(1.0f);
	ubo_data.projectionMatrix = glm::ortho(-SystemContext::screen.ratio, SystemContext::screen.ratio, -1.0f, 1.0f);
	ui_ubo->set_data(&ubo_data, sizeof(UI_UBO), GL_DYNAMIC_DRAW);
}
void UI_Renderer::update() {
	current_slots_buffer_size = Inventory::INVENTORY_SIZE * 4;
	if (main_player_ptr->inventory.is_open) {
		current_slots_buffer_index_size = Inventory::INVENTORY_SIZE * 6;

		update_items(); //SHOULD PROBABLY CHANGE THIS METHOD

		//add chest slots
		if (main_player_ptr->inventory.has_open_chest) {
			slots_vbo->update_data(chest_slot_vertices.data(), chest_slot_vertices.size() * sizeof(UI_Vertex2f), current_slots_buffer_size * sizeof(UI_Vertex2f));
			current_slots_buffer_size += Inventory::INVENTORY_CHEST_SIZE * 4;
			current_slots_buffer_index_size += Inventory::INVENTORY_CHEST_SIZE * 6;
		}
		//add tooltip
		if (main_player_ptr->inventory.tooltip_is_visible) {
			update_tooltip_data();
			slots_vbo->update_data(tooltip_vertices.data(), tooltip_vertices.size() * sizeof(UI_Vertex2f), current_slots_buffer_size * sizeof(UI_Vertex2f));
		}
	}
	else {
		current_slots_buffer_index_size = 10 * 6;
		update_hotbar_active_slot();
		slots_vbo->update_data(active_hotbar_slot_vertices.data(), active_hotbar_slot_vertices.size() * sizeof(UI_Vertex2f), current_slots_buffer_size * sizeof(UI_Vertex2f));
	}
}
void UI_Renderer::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	//slots
	renderer->renderIndexedData(ui_shader, slots_vao, slots_vbo, ebo, current_slots_buffer_index_size);

	if (main_player_ptr->inventory.is_open) {
		//items
		renderer->renderIndexedData(ui_shader, items_vao, items_vbo, ebo, current_items_buffer_index_size);
		//items text
		renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, current_text_buffer_index_size);
		//tooltip
		if (main_player_ptr->inventory.tooltip_is_visible) {
			//tooltip background
			renderer->renderIndexedData(ui_shader, slots_vao, slots_vbo, ebo, 9 * 6, current_slots_buffer_index_size);
			//tooltip info
			renderer->renderIndexedData(sdf_text_shader, tooltip_text_vao, tooltip_text_vbo, ebo, current_tooltip_text_index_size);
		}
	}
	else {
		//render active slot
		renderer->renderIndexedData(ui_shader, slots_vao, slots_vbo, ebo, 6, Inventory::INVENTORY_SIZE * 6);
		//hotbar items
		renderer->renderIndexedData(ui_shader, items_vao, items_vbo, ebo, current_items_buffer_hotbar_index_size);
		//hotbar items text
		renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, current_text_buffer_hotbar_index_size);
	}
}

void UI_Renderer::init_basic_inventory_slots_data() {
	float Xpos;
	float Ypos = 0.83f;
	float slot_size = 0.1f;
	glm::vec2* uv_ptr = spriteMgr->get_sprite(1).UV;

	auto add_slot_to_buffer = [&]() {
		basic_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		basic_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		basic_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		basic_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
		main_player_ptr->inventory.add_inventory_basic_slot_bounds(Xpos, Xpos + slot_size, Ypos, Ypos + slot_size);
	};
	auto add_chest_slot_to_buffer = [&]() {
		chest_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		chest_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		chest_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		chest_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
		main_player_ptr->inventory.add_inventory_chest_slot_bounds(Xpos, Xpos + slot_size, Ypos, Ypos + slot_size);
	};

	//main 50 slots
	for (int i = 0; i < 5; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f;
		for (int j = 0; j < 10; j++) {
			add_slot_to_buffer();
			Xpos += 0.11f;
		}
		Ypos -= 0.11f;
	}
	//8 slots (4 for coins and ammo)
	Xpos = -SystemContext::screen.ratio * 0.97f + 10.f * 0.11f;
	for (int i = 0; i < 2; i++) {
		Ypos = 0.72f;
		for (int j = 0; j < 4; j++) {
			add_slot_to_buffer();
			Ypos -= 0.11f;
		}
		Xpos += 0.11f;
	}
	//16 slots for armor and accessories
	Xpos = SystemContext::screen.ratio * 0.86f;
	for (int i = 0; i < 2; i++) {
		Ypos = 0.72f;
		for (int j = 0; j < 8; j++) {
			add_slot_to_buffer();
			Ypos -= 0.11f;
		}
		Xpos += 0.11f;
	}
	//40 slots for chests (used with inventory when any chest is open by the player)
	uv_ptr = spriteMgr->get_sprite(2).UV;
	Ypos = 0.83f - 0.11 * 5.f;
	for (int i = 0; i < 4; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f;
		for (int j = 0; j < 10; j++) {
			add_chest_slot_to_buffer();
			Xpos += 0.11f;
		}
		Ypos -= 0.11f;
	}
	slots_vbo->update_data(basic_slot_vertices.data(), sizeof(UI_Vertex2f) * basic_slot_vertices.size());
}

void UI_Renderer::update_tooltip_data() {
	TooltipData& tooltip_data = main_player_ptr->inventory.tooltipData;
	float x0 = 0.f, y0 = 0.f, x1 = 0.f, y1 = 0.f;
	float inner_height = tooltip_text_height;
	float inner_width;

	//update tooltip text
	sdf_tooltip_text_buffer.clear();
	current_tooltip_text_index_size = 0;
	inner_width = place_text_to_tooltip_buffer(
		tooltip_data.item_name.c_str(),
		tooltip_data.item_name.size(),
		glm::vec2(tooltip_frame_size, tooltip_frame_size),
		tooltip_text_height,
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
	);
	inner_width += tooltip_text_height;

	float start_x = SystemContext::mouse.ortho_x_pos + 0.03f;
	if (start_x + inner_width + tooltip_frame_size * 2.f > SystemContext::screen.ratio)
		start_x = SystemContext::mouse.ortho_x_pos - 0.01f - inner_width - tooltip_frame_size * 2.f;
	float start_y = SystemContext::mouse.ortho_y_pos - inner_height;
	adjust_tooltip_text_pos(start_x, start_y);
	tooltip_text_vbo->update_data(sdf_tooltip_text_buffer.data(), sdf_tooltip_text_buffer.size() * sizeof(UI_Vertex2f));
	
	//update tooltip background
	tooltip_vertices.clear();
	int first_sprite_id = 4;
	glm::vec2* uv_ptr = nullptr;

	auto add_tooltip_part_to_buffer = [&]() {
		tooltip_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x0, y0), uv_ptr[0] });
		tooltip_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x0, y1), uv_ptr[1] });
		tooltip_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x1, y1), uv_ptr[2] });
		tooltip_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x1, y0), uv_ptr[3] });
	};
	//left bottom
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	x0 = start_x; x1 = x0 + tooltip_frame_size;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//left center
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//left top
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//center bottom
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	x0 = x1; x1 += inner_width;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//center
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//center top
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//right bottom
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	x0 = x1; x1 += tooltip_frame_size;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//right center
	uv_ptr = spriteMgr->get_sprite(first_sprite_id++).UV;
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//right top
	uv_ptr = spriteMgr->get_sprite(first_sprite_id).UV;
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
}

void UI_Renderer::update_hotbar_active_slot() {
	active_hotbar_slot_vertices.clear();
	float y = 0.82f;
	float x = -SystemContext::screen.ratio * 0.97f + 0.11f * main_player_ptr->inventory.current_active_hotbar_slot - 0.01f;
	float slot_size = 0.12f;
	glm::vec2* uv_ptr = spriteMgr->get_sprite(3).UV;
	active_hotbar_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x, y), uv_ptr[0] });
	active_hotbar_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x, y + slot_size), uv_ptr[1] });
	active_hotbar_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x + slot_size, y + slot_size), uv_ptr[2] });
	active_hotbar_slot_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x + slot_size, y), uv_ptr[3] });
}

float UI_Renderer::place_text_to_buffer(const char* text, uint32_t text_size, glm::vec2 left_bottom_pos, float height, glm::vec4 color) {
	float start_x = left_bottom_pos.x;
	float start_y = left_bottom_pos.y - sdf_font_manager.mainFont.descender * height;
	float glyph_width, glyph_height;
	float x0, x1, y0, y1;
	float x_offset = 0;

	for (uint32_t i = 0; i < text_size; i++) {
		GlyphData& glyph = sdf_font_manager.mainFont.glyphsData[(uint32_t)text[i] - 32U];
		x0 = start_x + x_offset + glyph.bearingX_normalized * height;
		x1 = x0 + glyph.width_normalized * height;
		y0 = start_y + (glyph.bearingY_normalized - glyph.height_normalized) * height;
		y1 = y0 + glyph.height_normalized * height;
		sdf_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y0), glm::vec2(glyph.U0, glyph.V0), color });
		sdf_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y1), glm::vec2(glyph.U0, glyph.V1), color });
		sdf_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y1), glm::vec2(glyph.U1, glyph.V1), color });
		sdf_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y0), glm::vec2(glyph.U1, glyph.V0), color });
		x_offset += height * glyph.advance_normalized;
	}
	current_text_buffer_size += 4 * text_size;
	current_text_buffer_index_size += 6 * text_size;
	return x_offset;
}

float UI_Renderer::place_text_to_tooltip_buffer(const char* text, uint32_t text_size, glm::vec2 left_bottom_pos, float height, glm::vec4 color) {
	float start_x = left_bottom_pos.x;
	float start_y = left_bottom_pos.y - sdf_font_manager.mainFont.descender * height;
	float glyph_width, glyph_height;
	float x0, x1, y0, y1;
	float x_offset = 0;
	for (uint32_t i = 0; i < text_size; i++) {
		GlyphData& glyph = sdf_font_manager.mainFont.glyphsData[(uint32_t)text[i] - 32U];
		x0 = start_x + x_offset + glyph.bearingX_normalized * height;
		x1 = x0 + glyph.width_normalized * height;
		y0 = start_y + (glyph.bearingY_normalized - glyph.height_normalized) * height;
		y1 = y0 + glyph.height_normalized * height;
		sdf_tooltip_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y0), glm::vec2(glyph.U0, glyph.V0), color });
		sdf_tooltip_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y1), glm::vec2(glyph.U0, glyph.V1), color });
		sdf_tooltip_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y1), glm::vec2(glyph.U1, glyph.V1), color });
		sdf_tooltip_text_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y0), glm::vec2(glyph.U1, glyph.V0), color });
		x_offset += height * glyph.advance_normalized;
	}
	current_tooltip_text_index_size += 6 * text_size;
	return x_offset;
}

void UI_Renderer::adjust_tooltip_text_pos(float dX, float dY) {
	int size = sdf_tooltip_text_buffer.size() / 4;
	UI_Vertex2f* ptr = sdf_tooltip_text_buffer.data();
	for (int i = 0; i < size; i++) {
		ptr->pos.x += dX;
		ptr->pos.y += dY;
		ptr++;
		ptr->pos.x += dX;
		ptr->pos.y += dY;
		ptr++;
		ptr->pos.x += dX;
		ptr->pos.y += dY;
		ptr++;
		ptr->pos.x += dX;
		ptr->pos.y += dY;
		ptr++;
	}
}

void UI_Renderer::reset_text_buffer() {
	current_text_buffer_size = 0;
	current_text_buffer_index_size = 0;
	current_text_buffer_hotbar_index_size = 0;
	sdf_text_buffer.clear();
}

void UI_Renderer::update_items() {
	glm::vec2* uv_ptr;
	float item_size = 0.075f;
	float x, y;
	uint32_t text_size;

	if (main_player_ptr->inventory.should_update_base_items) {
		slot_items_vertices.clear();
		current_items_buffer_hotbar_index_size = 0;
		reset_text_buffer();

		for (int i = 0; i < Inventory::INVENTORY_SIZE; i++) {
			InventorySlot& slot = main_player_ptr->inventory.items[i];
			if (slot.item_id == 0) continue;

			ObjectInfo& obj_info = *ObjectsDB::objectInfo[slot.item_id];
			uv_ptr = spriteMgr->get_sprite(obj_info.sprite_id).UV;

			glm::vec4& slot_borders = main_player_ptr->inventory.inventory_slots_bounds[i];
			x = slot_borders.x + 0.0125f;
			y = slot_borders.z + 0.0125f;
			slot_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x, y), uv_ptr[0] });
			slot_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x, y + item_size), uv_ptr[1] });
			slot_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x + item_size, y + item_size), uv_ptr[2] });
			slot_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x + item_size, y), uv_ptr[3] });

			if (slot.amount > 1) {
				text_builder.add_int((int)slot.amount);
				text_size = text_builder.size();
				place_text_to_buffer(text_builder.data(), text_size, glm::vec2(slot_borders.x - 0.005f, slot_borders.z), 0.035, glm::vec4(1.0f));
				text_builder.reset();
			}

			if (i < 10) {
				current_items_buffer_hotbar_index_size += 6;
				if(slot.amount > 1)
					current_text_buffer_hotbar_index_size += 6 * text_size;
			}
		}
		current_items_buffer_size = slot_items_vertices.size();
		current_items_buffer_index_size = (current_items_buffer_size / 4) * 6;
		items_vbo->update_data(slot_items_vertices.data(), current_items_buffer_size * sizeof(UI_Vertex2f));
		text_vbo->update_data(sdf_text_buffer.data(), sdf_text_buffer.size() * sizeof(UI_Vertex2f));
		main_player_ptr->inventory.should_update_base_items = false;
	}

	if (main_player_ptr->inventory.should_update_chest_items && main_player_ptr->inventory.active_chest_items_ptr) {
		chest_items_vertices.clear();

		for (int i = 0; i < Inventory::INVENTORY_CHEST_SIZE; i++) {
			InventorySlot& slot = main_player_ptr->inventory.active_chest_items_ptr[i];
			if (slot.item_id == 0) continue;

			ObjectInfo& obj_info = *ObjectsDB::objectInfo[slot.item_id];
			uv_ptr = spriteMgr->get_sprite(obj_info.sprite_id).UV;

			glm::vec4& slot_borders = main_player_ptr->inventory.inventory_chest_slots_bounds[i];
			x = slot_borders.x + 0.0125f;
			y = slot_borders.z + 0.0125f;
			chest_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x, y), uv_ptr[0] });
			chest_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x, y + item_size), uv_ptr[1] });
			chest_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x + item_size, y + item_size), uv_ptr[2] });
			chest_items_vertices.emplace_back(UI_Vertex2f{ glm::vec2(x + item_size, y), uv_ptr[3] });
		}
		main_player_ptr->inventory.should_update_chest_items = false;
	}
}