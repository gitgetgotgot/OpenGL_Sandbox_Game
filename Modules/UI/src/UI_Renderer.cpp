#include <UI/UI_Renderer.h>
#include <Entities/Player.h>
#include <IOSystem/SystemContext.h>

uint32_t UI_Renderer::tooltip_slots_INDEX_SIZE = 9 * 6;

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

	icons_vao = std::make_unique<VAO>();
	icons_vao->bind_VAO();
	icons_vbo = std::make_unique<VBO>();
	icons_vbo->set_data(nullptr, 4 * 100 * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW); //100 icons
	ebo->bind_EBO();
	icons_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	icons_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	icons_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	icons_vao->unbind_VAO();
	icons_vbo->unbind_VBO();
	ebo->unbind_EBO();

	const uint32_t MAX_CRAFT_SLOTS_VERTEX_SIZE = 200 * 4;
	craft_slots_vao = std::make_unique<VAO>();
	craft_slots_vao->bind_VAO();
	craft_slots_vbo = std::make_unique<VBO>();
	craft_slots_vbo->set_data(nullptr, MAX_CRAFT_SLOTS_VERTEX_SIZE * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	craft_slots_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	craft_slots_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	craft_slots_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	craft_slots_vao->unbind_VAO();
	craft_slots_vbo->unbind_VBO();
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

	craft_items_vao = std::make_unique<VAO>();
	craft_items_vao->bind_VAO();
	craft_items_vbo = std::make_unique<VBO>();
	craft_items_vbo->set_data(nullptr, MAX_CRAFT_SLOTS_VERTEX_SIZE * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	craft_items_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	craft_items_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	craft_items_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	craft_items_vao->unbind_VAO();
	craft_items_vbo->unbind_VBO();
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

	craft_text_vao = std::make_unique<VAO>();
	craft_text_vao->bind_VAO();
	craft_text_vbo = std::make_unique<VBO>();
	craft_text_vbo->set_data(nullptr, 29 * 4 * 4 * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();
	craft_text_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	craft_text_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	craft_text_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	craft_text_vao->unbind_VAO();
	craft_text_vbo->unbind_VBO();
	ebo->unbind_EBO();

	ui_ubo = std::make_unique<UBO>();
	ui_ubo->bind_UBO(1);
	ubo_data.viewMatrix = glm::mat4(1.0f);
	ubo_data.projectionMatrix = glm::ortho(-SystemContext::screen.ratio, SystemContext::screen.ratio, -1.0f, 1.0f);
	ui_ubo->set_data(&ubo_data, sizeof(UI_UBO), GL_DYNAMIC_DRAW);

	active_hotbar_slot_buffer.reserve(4);
	basic_slot_buffer.reserve(Inventory::INVENTORY_SIZE * 4);
	chest_slot_buffer.reserve(Inventory::INVENTORY_CHEST_SIZE * 4);
	craft_info_slot_buffer.reserve(24 * 4);
	helper_craft_slot_buffer.reserve(20 * 8 * 4);
	craft_borders_buffer.reserve(2 * 4);
	slots_VERTEX_SIZE = Inventory::INVENTORY_SIZE * 4 + Inventory::INVENTORY_CHEST_SIZE * 4;
	base_slots_INDEX_SIZE = Inventory::INVENTORY_SIZE * 6 + Inventory::INVENTORY_CHEST_SIZE * 6;
	tooltip_buffer.reserve(9 * 4);
	icons_buffer.reserve(100 * 4);

	basic_items_buffer.reserve(Inventory::INVENTORY_SIZE * 4);
	chest_items_buffer.reserve(Inventory::INVENTORY_CHEST_SIZE * 4);
	craft_items_buffer.reserve(6 * 4);
	craft_info_items_buffer.reserve(24 * 4);
	craft_helper_items_buffer.reserve(160 * 4);
	cursor_item_buffer.reserve(4);
	cursor_item_text_buffer.reserve(4 * 4); //max number is 9999

	basic_text_buffer.reserve(Inventory::INVENTORY_SIZE * 4 * 4); //for each slot max number is 9999
	chest_text_buffer.reserve(Inventory::INVENTORY_CHEST_SIZE * 4 * 4);
}

void UI_Renderer::update() {
	update_icons();
	update_items(); //maybe can change this method

	if (main_player_ptr->inventory.is_open) {
		slots_INDEX_SIZE = Inventory::INVENTORY_SIZE * 6;
		//add chest slots
		if (main_player_ptr->inventory.has_open_chest) {
			slots_INDEX_SIZE += Inventory::INVENTORY_CHEST_SIZE * 6;
		}

		update_craft_slots();

		//add tooltip
		if (main_player_ptr->inventory.tooltip_is_visible) {
			update_tooltip_data();
		}
		//cursor item
		else if (main_player_ptr->inventory.cursor_item.item_id > 0) {
			update_cursor_item();
		}
	}
	else {
		slots_INDEX_SIZE = 10 * 6;
		update_hotbar_active_slot();
	}
}

void UI_Renderer::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	//slots
	renderer->renderIndexedData(ui_shader, slots_vao, slots_vbo, ebo, slots_INDEX_SIZE);

	if (main_player_ptr->inventory.is_open) {
		//items
		renderer->renderIndexedData(ui_shader, items_vao, items_vbo, ebo, TOTAL_ITEMS_INDEX_SIZE);

		//items text
		renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, TOTAL_TEXT_INDEX_SIZE);

		//craft UI
		if (main_player_ptr->inventory.current_crafts_available > 0) {
			uint32_t rect_y = (1.f + main_player_ptr->inventory.craft_slots_scissor_rect_y0) * 0.5f * SystemContext::screen.height;
			uint32_t rect_height = main_player_ptr->inventory.craft_slots_scissor_rect_height * 0.5f * SystemContext::screen.height;
			
			//render craft borders
			renderer->renderIndexedData(ui_shader, craft_slots_vao, craft_slots_vbo, ebo, 12);

			//scissor rect
			renderer->useScissorTest(true);
			renderer->setScissorRect(0, rect_y,SystemContext::screen.width, rect_height);
				//--craft slots
				renderer->renderIndexedData(ui_shader, craft_slots_vao, craft_slots_vbo, ebo, craft_slots_INDEX_SIZE, 12);
				//--craft items
				renderer->renderIndexedData(ui_shader, craft_items_vao, craft_items_vbo, ebo, craft_items_INDEX_SIZE);
			renderer->useScissorTest(false);
			
			//craft info slots
			renderer->renderIndexedData(ui_shader, craft_slots_vao, craft_slots_vbo, ebo, craft_info_slots_INDEX_SIZE, 12 + craft_slots_INDEX_SIZE);

			//helper craft slots
			if (main_player_ptr->inventory.helper_slots_are_visible)
				renderer->renderIndexedData(ui_shader, craft_slots_vao, craft_slots_vbo, ebo, helper_slots_INDEX_SIZE, 12 + craft_slots_INDEX_SIZE + craft_info_slots_INDEX_SIZE);

			//craft info items
			renderer->renderIndexedData(ui_shader, craft_items_vao, craft_items_vbo, ebo, craft_info_items_INDEX_SIZE, craft_items_INDEX_SIZE);
			
			//helper craft items
			if (main_player_ptr->inventory.helper_slots_are_visible)
				renderer->renderIndexedData(ui_shader, craft_items_vao, craft_items_vbo, ebo, craft_helper_items_INDEX_SIZE, craft_items_INDEX_SIZE + craft_info_items_INDEX_SIZE);

			//scissor rect
			renderer->useScissorTest(true);
			renderer->setScissorRect(0, rect_y, SystemContext::screen.width, rect_height);
				//--craft text
				renderer->renderIndexedData(sdf_text_shader, craft_text_vao, craft_text_vbo, ebo, craft_text_INDEX_SIZE);
			renderer->useScissorTest(false);
		}

		//tooltip
		if (main_player_ptr->inventory.tooltip_is_visible) {
			//tooltip background
			renderer->renderIndexedData(ui_shader, slots_vao, slots_vbo, ebo, tooltip_slots_INDEX_SIZE, base_slots_INDEX_SIZE);
			//tooltip info
			renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, tooltip_text_INDEX_SIZE, TOTAL_TEXT_INDEX_SIZE);
		}

		//cursor item
		else if (main_player_ptr->inventory.cursor_item.item_id > 0) {
			//render item
			renderer->renderIndexedData(ui_shader, items_vao, items_vbo, ebo, 6, TOTAL_ITEMS_INDEX_SIZE);
			//render text
			renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, cursor_text_INDEX_SIZE, TOTAL_TEXT_INDEX_SIZE);
		}
	}
	else {
		//render active slot
		renderer->renderIndexedData(ui_shader, slots_vao, slots_vbo, ebo, 6, base_slots_INDEX_SIZE);

		//hotbar items
		renderer->renderIndexedData(ui_shader, items_vao, items_vbo, ebo, items_hotbar_INDEX_SIZE);

		//hotbar items text
		renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, text_hotbar_INDEX_SIZE);

		//active item name
		renderer->renderIndexedData(sdf_text_shader, text_vao, text_vbo, ebo, tooltip_text_INDEX_SIZE, TOTAL_TEXT_INDEX_SIZE);
	}

	//icons
	renderer->renderIndexedData(ui_shader, icons_vao, icons_vbo, ebo, icons_INDEX_SIZE);
}

void UI_Renderer::init_basic_inventory_slots_data() {
	float Xpos;
	float Ypos = 0.83f;
	float slot_size = 0.1f;
	float border_width, border_height;
	glm::vec2* uv_ptr = spriteMgr->get_sprite(1).UV;

	auto add_slot_to_buffer = [&]() {
		basic_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		basic_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		basic_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		basic_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
		main_player_ptr->inventory.add_basic_slot_bounds(Xpos, Xpos + slot_size, Ypos, Ypos + slot_size);
	};
	auto add_chest_slot_to_buffer = [&]() {
		chest_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		chest_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		chest_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		chest_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
		main_player_ptr->inventory.add_chest_slot_bounds(Xpos, Xpos + slot_size, Ypos, Ypos + slot_size);
	};
	auto add_helper_slot_to_buffer = [&]() {
		helper_craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		helper_craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		helper_craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		helper_craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
		main_player_ptr->inventory.add_helper_slot_bounds(Xpos, Xpos + slot_size, Ypos, Ypos + slot_size);
		};
	auto add_craft_border_to_buffer = [&]() {
		craft_borders_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		craft_borders_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + border_height), uv_ptr[1] });
		craft_borders_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + border_width, Ypos + border_height), uv_ptr[2] });
		craft_borders_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + border_width, Ypos), uv_ptr[3] });
		};
	auto add_craft_info_slot_to_buffer = [&]() {
		craft_info_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		craft_info_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		craft_info_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		craft_info_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
		main_player_ptr->inventory.add_craft_info_slot_bounds(Xpos, Xpos + slot_size, Ypos, Ypos + slot_size);
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
	Xpos = SystemContext::screen.ratio * 0.82f;
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
	slots_vbo->update_data(basic_slot_buffer.data(), sizeof(UI_Vertex2f) * basic_slot_buffer.size());
	slots_vbo->update_data(chest_slot_buffer.data(), chest_slot_buffer.size() * sizeof(UI_Vertex2f), basic_slot_buffer.size() * sizeof(UI_Vertex2f));
	
	//15 slots for craft info slots
	uv_ptr = spriteMgr->get_sprite(1).UV;
	Ypos = 0.83f - 0.11 * 10.f + 0.005f - 0.14f * 2.f;
	for (int i = 0; i < 3; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f + 0.2f;
		for (int j = 0; j < 8; j++) {
			add_craft_info_slot_to_buffer();
			Xpos += 0.11f;
		}
		Ypos -= 0.11f;
	}

	//20 X 8 slots for helper crafting slots
	slot_size = 0.08f;
	Ypos = 0.83f - 0.11 * 9.f + 0.02;
	for (int i = 0; i < 8; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f + 10.f * 0.11f;
		for (int j = 0; j < 20; j++) {
			add_helper_slot_to_buffer();
			Xpos += 0.09f;
		}
		Ypos -= 0.09f;
	}
	
	//craft borders
	Xpos = -SystemContext::screen.ratio * 0.97f - 0.03f;
	Ypos = 0.83f - 0.11 * 10.f + 0.125f;
	border_width = 0.28f; border_height = 0.07f;
	uv_ptr = spriteMgr->get_sprite(5).UV;
	add_craft_border_to_buffer();
	Ypos -= 0.14f * 5.5f;
	uv_ptr = spriteMgr->get_sprite(6).UV;
	add_craft_border_to_buffer();
	craft_slots_vbo->update_data(craft_borders_buffer.data(), craft_borders_buffer.size() * sizeof(UI_Vertex2f));
}

void UI_Renderer::init_icons_base_vertices() {
	float Xpos;
	float Ypos = 0.9f;
	float icon_size = 0.06f;
	glm::vec2* uv_ptr = spriteMgr->get_sprite(16).UV;

	auto add_icon_to_buffer = [&]() {
		icons_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		icons_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + icon_size), uv_ptr[1] });
		icons_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + icon_size, Ypos + icon_size), uv_ptr[2] });
		icons_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + icon_size, Ypos), uv_ptr[3] });
		};

	//hearts vertices
	for (int i = 0; i < 2; i++) {
		Xpos = SystemContext::screen.ratio * 0.58f;
		for (int j = 0; j < 10; j++) {
			add_icon_to_buffer();
			Xpos += 0.061f;
		}
		Ypos -= 0.061f;
	}
	//mana stars vertices
	Xpos = SystemContext::screen.ratio * 0.95f;
	Ypos = 0.9f;
	uv_ptr = spriteMgr->get_sprite(19).UV;
	for (int i = 0; i < 10; i++) {
		add_icon_to_buffer();
		Ypos -= 0.061f;
	}
}

void UI_Renderer::update_tooltip_data() {
	TooltipData& tooltip_data = main_player_ptr->inventory.tooltipData;
	ObjectInfo& item_info = *ObjectsDB::objectInfo[tooltip_data.item_id];
	float x0 = 0.f, y0 = 0.f, x1 = 0.f, y1 = 0.f;
	float inner_height = tooltip_frame_size; //y offset
	float inner_width;
	glm::vec2 text_pos(tooltip_frame_size, tooltip_frame_size * 2.f); //positions of text are going from top to bottom

	auto add_info_text = [&](const char* text, uint32_t text_size, glm::vec4 color = glm::vec4(1.0f)) {
		float text_ortho_length = place_text_to_buffer(tooltip_text_buffer, text, text_size, text_pos, tooltip_text_height, color);
		inner_width = text_ortho_length > inner_width ? text_ortho_length : inner_width;
		tooltip_text_INDEX_SIZE += text_size * 6;
		text_pos.y -= tooltip_text_height * 1.1f;
	};

	//update tooltip text
	tooltip_text_buffer.clear();
	tooltip_text_INDEX_SIZE = 0;
	switch (item_info.objectType) {
	case ObjectType::isBlock:
		text_pos.y += tooltip_text_height * 1.1f;
		inner_height += tooltip_text_height * 1.1f * 2;
		add_info_text(item_info.name.c_str(), item_info.name.size());
		add_info_text("Can be placed", 13);
		break;
	case ObjectType::isComplexObject:
		text_pos.y += tooltip_text_height * 1.1f;
		inner_height += tooltip_text_height * 1.1f * 2;
		add_info_text(item_info.name.c_str(), item_info.name.size());
		add_info_text("Can be placed", 13);
		break;
	case ObjectType::isWeapon:
		text_pos.y += tooltip_text_height * 1.1f * 2;
		inner_height += tooltip_text_height * 1.1f * 3;
		add_info_text(item_info.name.c_str(), item_info.name.size(), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text_builder.add_int(item_info.get_damage()).add_text(" damage");
		add_info_text(text_builder.data(), text_builder.size());
		text_builder.reset();
		text_builder.add_int(item_info.get_crit_chance()).add_text("% crit chance");
		add_info_text(text_builder.data(), text_builder.size());
		text_builder.reset();
		break;
	default:
		inner_height += tooltip_text_height * 1.1f;
		add_info_text(item_info.name.c_str(), item_info.name.size());
		break;
	}
	inner_width += tooltip_frame_size; //x offset

	float start_x = SystemContext::mouse.ortho_x_pos + 0.03f;
	if (start_x + inner_width + tooltip_frame_size * 2.f > SystemContext::screen.ratio)
		start_x = SystemContext::mouse.ortho_x_pos - 0.01f - inner_width - tooltip_frame_size * 2.f;
	float start_y = SystemContext::mouse.ortho_y_pos - inner_height;
	adjust_tooltip_text_pos(start_x, start_y);
	text_vbo->update_data(tooltip_text_buffer.data(), tooltip_text_buffer.size() * sizeof(UI_Vertex2f), TOTAL_TEXT_VERTEX_SIZE * sizeof(UI_Vertex2f));
	
	//update tooltip background
	tooltip_buffer.clear();
	int first_sprite_id = 7;
	glm::vec2* uv_ptr = nullptr;

	auto add_tooltip_part_to_buffer = [&]() {
		tooltip_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y0), uv_ptr[0] });
		tooltip_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y1), uv_ptr[1] });
		tooltip_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y1), uv_ptr[2] });
		tooltip_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y0), uv_ptr[3] });
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

	slots_vbo->update_data(tooltip_buffer.data(), tooltip_buffer.size() * sizeof(UI_Vertex2f), slots_VERTEX_SIZE * sizeof(UI_Vertex2f));
}

void UI_Renderer::update_cursor_item() {
	float item_size = 0.075f;
	float start_x = SystemContext::mouse.ortho_x_pos - item_size;
	float start_y = SystemContext::mouse.ortho_y_pos - item_size * 0.5f;
	InventorySlot& cursor_slot = main_player_ptr->inventory.cursor_item;

	ObjectInfo& obj_info = *ObjectsDB::objectInfo[cursor_slot.item_id];
	glm::vec2* uv_ptr = spriteMgr->get_sprite(obj_info.sprite_id).UV;
	
	cursor_text_INDEX_SIZE = 0;
	cursor_item_text_buffer.clear();
	uint32_t text_size = 0;
	if (cursor_slot.amount > 1) {
		text_builder.add_int(cursor_slot.amount);
		text_size = text_builder.size();
		cursor_text_INDEX_SIZE = text_size * 6;
		place_text_to_buffer(cursor_item_text_buffer, text_builder.data(), text_builder.size(), glm::vec2(start_x - 0.0175f, start_y - 0.0125f), 0.035f, glm::vec4(1.0f));
		text_builder.reset();
		text_vbo->update_data(cursor_item_text_buffer.data(), cursor_item_text_buffer.size() * sizeof(UI_Vertex2f), TOTAL_TEXT_VERTEX_SIZE * sizeof(UI_Vertex2f));
	}

	cursor_item_buffer.clear();
	float sizeX = item_size, sizeY = item_size;
	if (obj_info.objectType == ObjectType::isComplexObject) {
		glm::vec2 size_ratio = obj_info.get_norm_size_ratio();
		start_x += (1.0f - size_ratio.x) * item_size * 0.5f;
		start_y += (1.0f - size_ratio.y) * item_size * 0.5f;
		sizeX *= size_ratio.x;
		sizeY *= size_ratio.y;
	}
	cursor_item_buffer.emplace_back(UI_Vertex2f{ glm::vec2(start_x, start_y), uv_ptr[0] });
	cursor_item_buffer.emplace_back(UI_Vertex2f{ glm::vec2(start_x, start_y + sizeY), uv_ptr[1] });
	cursor_item_buffer.emplace_back(UI_Vertex2f{ glm::vec2(start_x + sizeX, start_y + sizeY), uv_ptr[2] });
	cursor_item_buffer.emplace_back(UI_Vertex2f{ glm::vec2(start_x + sizeX, start_y), uv_ptr[3] });
	items_vbo->update_data(cursor_item_buffer.data(), cursor_item_buffer.size() * sizeof(UI_Vertex2f), TOTAL_ITEMS_VERTEX_SIZE * sizeof(UI_Vertex2f));
}

void UI_Renderer::update_hotbar_active_slot() {
	active_hotbar_slot_buffer.clear();
	float y = 0.825f;
	float x = -SystemContext::screen.ratio * 0.97f + 0.11f * main_player_ptr->inventory.current_active_hotbar_slot - 0.005f;
	float slot_size = 0.11f;
	glm::vec2* uv_ptr = spriteMgr->get_sprite(3).UV;
	active_hotbar_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x, y), uv_ptr[0] });
	active_hotbar_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x, y + slot_size), uv_ptr[1] });
	active_hotbar_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x + slot_size, y + slot_size), uv_ptr[2] });
	active_hotbar_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(x + slot_size, y), uv_ptr[3] });

	slots_vbo->update_data(active_hotbar_slot_buffer.data(), active_hotbar_slot_buffer.size() * sizeof(UI_Vertex2f), slots_VERTEX_SIZE * sizeof(UI_Vertex2f));

	uint16_t active_item_id = main_player_ptr->inventory.get_active_item_id();
	tooltip_text_buffer.clear();
	tooltip_text_INDEX_SIZE = 0;

	if (active_item_id != 0) {
		ObjectInfo& item_info = *ObjectsDB::objectInfo[active_item_id];
		place_text_to_buffer(tooltip_text_buffer, item_info.name.c_str(), item_info.name.size(),
			glm::vec2(-SystemContext::screen.ratio * 0.97f, 0.945f), tooltip_text_height, glm::vec4(1.0f));
		text_vbo->update_data(tooltip_text_buffer.data(), tooltip_text_buffer.size() * sizeof(UI_Vertex2f), TOTAL_TEXT_VERTEX_SIZE * sizeof(UI_Vertex2f));
		tooltip_text_INDEX_SIZE = item_info.name.size() * 6;
	}
}

float UI_Renderer::place_text_to_buffer(std::vector<UI_Vertex2f>& buffer, const char* text, uint32_t text_size, glm::vec2 left_bottom_pos, float height, glm::vec4 color) {
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
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y0), glm::vec2(glyph.U0, glyph.V0), color });
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(x0, y1), glm::vec2(glyph.U0, glyph.V1), color });
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y1), glm::vec2(glyph.U1, glyph.V1), color });
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(x1, y0), glm::vec2(glyph.U1, glyph.V0), color });
		x_offset += height * glyph.advance_normalized;
	}
	return x_offset;
}

void UI_Renderer::adjust_tooltip_text_pos(float dX, float dY) {
	int size = tooltip_text_buffer.size() / 4;
	UI_Vertex2f* ptr = tooltip_text_buffer.data();
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

void UI_Renderer::update_items() {
	Inventory& inv = main_player_ptr->inventory;

	if (inv.should_update_base_items || inv.should_update_chest_items) {
		glm::vec2* uv_ptr;
		float item_size = 0.075f;
		float x, y;
		uint32_t text_size;
		bool should_update_buffer = false;

		auto add_item_to_buffer = [&](std::vector<UI_Vertex2f>& buffer, ObjectInfo& item_info) {
			float sizeX = item_size, sizeY = item_size;
			if (item_info.objectType == ObjectType::isComplexObject) {
				glm::vec2 size_ratio = item_info.get_norm_size_ratio();
				x += (1.0f - size_ratio.x) * item_size * 0.5f;
				y += (1.0f - size_ratio.y) * item_size * 0.5f;
				sizeX *= size_ratio.x;
				sizeY *= size_ratio.y;
			}
			buffer.emplace_back(UI_Vertex2f{ glm::vec2(x, y), uv_ptr[0] });
			buffer.emplace_back(UI_Vertex2f{ glm::vec2(x, y + sizeY), uv_ptr[1] });
			buffer.emplace_back(UI_Vertex2f{ glm::vec2(x + sizeX, y + sizeY), uv_ptr[2] });
			buffer.emplace_back(UI_Vertex2f{ glm::vec2(x + sizeX, y), uv_ptr[3] });
			};

		if (main_player_ptr->inventory.should_update_base_items) {
			should_update_buffer = true;
			basic_items_buffer.clear();
			basic_text_buffer.clear();
			items_hotbar_INDEX_SIZE = 0;
			text_hotbar_INDEX_SIZE = 0;

			for (int i = 0; i < Inventory::INVENTORY_SIZE; i++) {
				InventorySlot& slot = main_player_ptr->inventory.items[i];
				if (slot.item_id == 0) continue;

				ObjectInfo& obj_info = *ObjectsDB::objectInfo[slot.item_id];
				uv_ptr = spriteMgr->get_sprite(obj_info.sprite_id).UV;

				glm::vec4& slot_borders = main_player_ptr->inventory.slots_bounds[i];
				x = slot_borders.x + 0.0125f;
				y = slot_borders.z + 0.0125f;
				add_item_to_buffer(basic_items_buffer, obj_info);

				if (slot.amount > 1) {
					text_builder.add_int((int)slot.amount);
					text_size = text_builder.size();
					place_text_to_buffer(basic_text_buffer, text_builder.data(), text_size, glm::vec2(slot_borders.x - 0.005f, slot_borders.z), 0.035, glm::vec4(1.0f));
					text_builder.reset();
				}

				if (i < 10) {
					items_hotbar_INDEX_SIZE += 6;
					if (slot.amount > 1)
						text_hotbar_INDEX_SIZE += 6 * text_size;
				}
			}
			items_VERTEX_SIZE = basic_items_buffer.size();
			items_INDEX_SIZE = (items_VERTEX_SIZE / 4) * 6;
			text_VERTEX_SIZE = basic_text_buffer.size();
			text_INDEX_SIZE = (text_VERTEX_SIZE / 4) * 6;
			main_player_ptr->inventory.should_update_base_items = false;
		}

		if (main_player_ptr->inventory.should_update_chest_items && main_player_ptr->inventory.active_chest_items_ptr) {
			should_update_buffer = true;
			chest_items_buffer.clear();
			chest_text_buffer.clear();

			for (int i = 0; i < Inventory::INVENTORY_CHEST_SIZE; i++) {
				InventorySlot& slot = main_player_ptr->inventory.active_chest_items_ptr[i];
				if (slot.item_id == 0) continue;

				ObjectInfo& obj_info = *ObjectsDB::objectInfo[slot.item_id];
				uv_ptr = spriteMgr->get_sprite(obj_info.sprite_id).UV;

				glm::vec4& slot_borders = main_player_ptr->inventory.chest_slots_bounds[i];
				x = slot_borders.x + 0.0125f;
				y = slot_borders.z + 0.0125f;
				add_item_to_buffer(chest_items_buffer, obj_info);

				if (slot.amount > 1) {
					text_builder.add_int((int)slot.amount);
					text_size = text_builder.size();
					place_text_to_buffer(chest_text_buffer, text_builder.data(), text_size, glm::vec2(slot_borders.x - 0.005f, slot_borders.z), 0.035, glm::vec4(1.0f));
					text_builder.reset();
				}
			}
			chest_items_VERTEX_SIZE = chest_items_buffer.size();
			chest_items_INDEX_SIZE = (chest_items_VERTEX_SIZE / 4) * 6;
			chest_text_VERTEX_SIZE = chest_text_buffer.size();
			chest_text_INDEX_SIZE = (chest_text_VERTEX_SIZE / 4) * 6;
			main_player_ptr->inventory.should_update_chest_items = false;
		}
	}

	TOTAL_ITEMS_INDEX_SIZE = items_INDEX_SIZE;
	TOTAL_TEXT_INDEX_SIZE = text_INDEX_SIZE;
	TOTAL_ITEMS_VERTEX_SIZE = items_VERTEX_SIZE;
	TOTAL_TEXT_VERTEX_SIZE = text_VERTEX_SIZE;
	//add basic items sprites and text
	items_vbo->update_data(basic_items_buffer.data(), items_VERTEX_SIZE * sizeof(UI_Vertex2f));
	text_vbo->update_data(basic_text_buffer.data(), text_VERTEX_SIZE * sizeof(UI_Vertex2f));
	//add chest items sprites and text
	if (main_player_ptr->inventory.has_open_chest) {
		TOTAL_ITEMS_INDEX_SIZE += chest_items_INDEX_SIZE;
		TOTAL_TEXT_INDEX_SIZE += chest_text_INDEX_SIZE;
		TOTAL_ITEMS_VERTEX_SIZE += chest_items_VERTEX_SIZE;
		TOTAL_TEXT_VERTEX_SIZE += chest_text_VERTEX_SIZE;
		items_vbo->update_data(chest_items_buffer.data(), chest_items_VERTEX_SIZE * sizeof(UI_Vertex2f), items_VERTEX_SIZE * sizeof(UI_Vertex2f));
		text_vbo->update_data(chest_text_buffer.data(), chest_text_VERTEX_SIZE * sizeof(UI_Vertex2f), text_VERTEX_SIZE * sizeof(UI_Vertex2f));
	}
}

void UI_Renderer::update_craft_slots() {
	if (main_player_ptr->inventory.current_crafts_available == 0) return;

	float Xpos, Ypos, slot_size;
	glm::vec2* uv_ptr = spriteMgr->get_sprite(4).UV;
	auto add_slot_to_buffer = [&]() {
		craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + slot_size), uv_ptr[1] });
		craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos + slot_size), uv_ptr[2] });
		craft_slot_buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + slot_size, Ypos), uv_ptr[3] });
	};
	auto add_item_to_buffer = [&](std::vector<UI_Vertex2f>& buffer, ObjectInfo& item_info) {
		float sizeX = slot_size, sizeY = slot_size;
		if (item_info.objectType == ObjectType::isComplexObject) {
			glm::vec2 size_ratio = item_info.get_norm_size_ratio();
			Xpos += (1.0f - size_ratio.x) * slot_size * 0.5f;
			Ypos += (1.0f - size_ratio.y) * slot_size * 0.5f;
			sizeX *= size_ratio.x;
			sizeY *= size_ratio.y;
		}
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos), uv_ptr[0] });
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos, Ypos + sizeY), uv_ptr[1] });
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + sizeX, Ypos + sizeY), uv_ptr[2] });
		buffer.emplace_back(UI_Vertex2f{ glm::vec2(Xpos + sizeX, Ypos), uv_ptr[3] });
	};

	craft_slot_buffer.clear();
	craft_items_buffer.clear();
	craft_info_items_buffer.clear();
	craft_helper_items_buffer.clear();
	craft_text_buffer.clear();

	int craft_slots_size = main_player_ptr->inventory.craft_slots_move ? 6 : 5;
	craft_slots_INDEX_SIZE = craft_slots_size * 6;

	craft_items_INDEX_SIZE = 0;
	craft_text_INDEX_SIZE = 0;
	uint32_t text_size = 0;
	for (int i = 0; i < craft_slots_size; i++) {
		DynamicSlot& slot = main_player_ptr->inventory.craft_slots[i];
		slot_size = slot.current_size;
		Xpos = slot.current_pos.x - slot_size * 0.5f;
		Ypos = slot.current_pos.y - slot_size * 0.5f;
		uv_ptr = spriteMgr->get_sprite(4).UV;
		add_slot_to_buffer();

		if (slot.item_data.amount > 1) {
			text_builder.add_int((int)slot.item_data.amount);
			text_size = text_builder.size();
			place_text_to_buffer(craft_text_buffer, text_builder.data(), text_size, glm::vec2(Xpos, Ypos), slot_size * 0.35f, glm::vec4(1.0f));
			text_builder.reset();
			craft_text_INDEX_SIZE += 6 * text_size;
		}

		Xpos += slot_size * 0.125f;
		Ypos += slot_size * 0.125f;
		slot_size *= 0.75f;

		if (slot.item_data.item_id > 1) {
			ObjectInfo& item_info = *ObjectsDB::objectInfo[slot.item_data.item_id];
			uv_ptr = spriteMgr->get_sprite(item_info.sprite_id).UV;
			add_item_to_buffer(craft_items_buffer, item_info);
			craft_items_INDEX_SIZE += 6;
		}
	}

	CraftableItem& craftable_item = main_player_ptr->inventory.get_craft_item(main_player_ptr->inventory.current_craftable_item_index);
	uint32_t info_slots_size = craftable_item.items_needed.size();

	uint32_t current_craft_slots_buf_size = 8;
	uint32_t current_item_slots_buf_size = 0;

	craft_slots_vbo->update_data( //add craft slots after craft borders
		craft_slot_buffer.data(),
		craft_slot_buffer.size() * sizeof(UI_Vertex2f),
		current_craft_slots_buf_size * sizeof(UI_Vertex2f)
	);
	current_craft_slots_buf_size += craft_slots_size * 4;

	craft_slots_vbo->update_data( //add info slots after craft slots
		craft_info_slot_buffer.data(),
		info_slots_size * 4 * sizeof(UI_Vertex2f),
		current_craft_slots_buf_size * sizeof(UI_Vertex2f)
	);

	craft_items_vbo->update_data(
		craft_items_buffer.data(),
		craft_items_buffer.size() * sizeof(UI_Vertex2f)
	);
	current_item_slots_buf_size += craft_items_buffer.size();
	craft_info_slots_INDEX_SIZE = craft_info_items_INDEX_SIZE = 0;

	if (craft_slots_size == 5) {
		craft_info_slots_INDEX_SIZE = info_slots_size * 6;
		craft_info_items_INDEX_SIZE = craft_info_slots_INDEX_SIZE;
		current_craft_slots_buf_size += info_slots_size * 4;

		for (int i = 0; i < info_slots_size; i++) {
			UI_Vertex2f& left_bottom_vertex = craft_info_slot_buffer[i * 4];
			Xpos = left_bottom_vertex.pos.x;
			Ypos = left_bottom_vertex.pos.y;
			slot_size = 0.1f;
			CraftingPair& craft_pair = craftable_item.items_needed[i];
			ObjectInfo& item_info = *ObjectsDB::objectInfo[craft_pair.item_id];
			uv_ptr = spriteMgr->get_sprite(item_info.sprite_id).UV;

			if (craft_pair.amount > 1) {
				text_builder.add_int((int)craft_pair.amount);
				text_size = text_builder.size();
				place_text_to_buffer(craft_text_buffer, text_builder.data(), text_size, glm::vec2(Xpos, Ypos), slot_size * 0.35f, glm::vec4(1.0f));
				text_builder.reset();
				craft_text_INDEX_SIZE += 6 * text_size;
			}

			Xpos += 0.0125f;
			Ypos += 0.0125f;
			slot_size = 0.075f;
			add_item_to_buffer(craft_info_items_buffer, item_info);
		}
		craft_items_vbo->update_data(
			craft_info_items_buffer.data(),
			info_slots_size * 4 * sizeof(UI_Vertex2f),
			current_item_slots_buf_size * sizeof(UI_Vertex2f)
		);
		current_item_slots_buf_size += info_slots_size * 4;
	}

	if (main_player_ptr->inventory.helper_slots_are_visible) {
		craft_slots_vbo->update_data( //add helper craft slots
			helper_craft_slot_buffer.data(),
			main_player_ptr->inventory.current_crafts_available * 4 * sizeof(UI_Vertex2f),
			current_craft_slots_buf_size * sizeof(UI_Vertex2f)
		);
		slot_size = 0.08f * 0.8f; float offset = 0.08f * 0.1f;
		for (int i = 0; i < main_player_ptr->inventory.current_crafts_available; i++) {
			UI_Vertex2f& left_bottom_vertex = helper_craft_slot_buffer[i * 4];
			Xpos = left_bottom_vertex.pos.x + offset;
			Ypos = left_bottom_vertex.pos.y + offset;
			ObjectInfo& item_info = *ObjectsDB::objectInfo[main_player_ptr->inventory.get_craft_item(i).item_id];
			uv_ptr = spriteMgr->get_sprite(item_info.sprite_id).UV;
			add_item_to_buffer(craft_helper_items_buffer, item_info);
		}
		craft_items_vbo->update_data(
			craft_helper_items_buffer.data(),
			craft_helper_items_buffer.size() * sizeof(UI_Vertex2f),
			current_item_slots_buf_size * sizeof(UI_Vertex2f)
		);
		helper_slots_INDEX_SIZE = main_player_ptr->inventory.current_crafts_available * 6;
		craft_helper_items_INDEX_SIZE = helper_slots_INDEX_SIZE;
	}

	craft_text_vbo->update_data(craft_text_buffer.data(), craft_text_buffer.size() * sizeof(UI_Vertex2f));
}

void UI_Renderer::update_icons() {
	icons_INDEX_SIZE = 6 * 30;
	icons_vbo->update_data(icons_buffer.data(), icons_buffer.size() * sizeof(UI_Vertex2f));
}