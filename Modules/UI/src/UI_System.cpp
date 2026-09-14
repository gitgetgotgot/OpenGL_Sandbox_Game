#include "UI/UI_System.h"
#include "UI/UI_ObjectManager.h"
#include "UI/FontManager.h"
#include "UI/CanvasManager.h"
#include "UI/UI_BehaviourSystem.h"
#include <IOSystem/SystemContext.h>

void CoreUI::UI_System::init() {
	FontManager::get_instance().load_main_sdf_font("verdana_SDF");

	const uint32_t MAX_SPRITES_VERTEX_SIZE = MAX_SPRITES_PER_DRAW * 4;
	const uint32_t MAX_SPRITES_INDEX_SIZE = MAX_SPRITES_PER_DRAW * 6;

	const uint32_t MAX_SDF_TEXT_VERTEX_SIZE = MAX_SDF_TEXT_PER_DRAW * 4;
	const uint32_t MAX_SDF_TEXT_INDEX_SIZE = MAX_SDF_TEXT_PER_DRAW * 6;

	// sprites shader & buffers
	sprites_shader = std::make_unique<ShaderProgram>("Resources/shaders/ui_sprites.vert", "Resources/shaders/ui_sprites.frag");

	//common EBO
	ebo = std::make_unique<EBO>();
	GLuint* sprite_index_buffer = new GLuint[MAX_SPRITES_INDEX_SIZE];
	for (int i = 0; i < MAX_SPRITES_PER_DRAW; i++) {
		sprite_index_buffer[i * 6] = sprite_index_buffer[i * 6 + 3] = i * 4;
		sprite_index_buffer[i * 6 + 2] = sprite_index_buffer[i * 6 + 4] = sprite_index_buffer[i * 6] + 2;
		sprite_index_buffer[i * 6 + 1] = sprite_index_buffer[i * 6 + 2] - 1;
		sprite_index_buffer[i * 6 + 5] = sprite_index_buffer[i * 6 + 2] + 1;
	}
	ebo->set_data(sprite_index_buffer, MAX_SPRITES_INDEX_SIZE * sizeof(GLuint), GL_STATIC_DRAW);
	delete[] sprite_index_buffer;

	sprites_vao = std::make_unique<VAO>();
	sprites_vao->bind_VAO();
	sprites_vbo = std::make_unique<VBO>();
	sprites_vbo->set_data(nullptr, MAX_SPRITES_VERTEX_SIZE * sizeof(UI_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();

	sprites_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)0);
	sprites_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(2 * sizeof(float)));
	sprites_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Vertex2f), (void*)(4 * sizeof(float)));
	sprites_vao->link_Attribute(3, 1, GL_UNSIGNED_INT, sizeof(UI_Vertex2f), (void*)(8 * sizeof(float)));

	sprites_vao->unbind_VAO();
	sprites_vbo->unbind_VBO();
	ebo->unbind_EBO();

	// SDF text shader & buffers
	sdf_text_shader = std::make_unique<ShaderProgram>("Resources/shaders/sdf_text.vert", "Resources/shaders/sdf_text.frag");

	sdf_text_vao = std::make_unique<VAO>();
	sdf_text_vao->bind_VAO();
	sdf_text_vbo = std::make_unique<VBO>();
	sdf_text_vbo->set_data(nullptr, MAX_SDF_TEXT_VERTEX_SIZE * sizeof(UI_Text_Vertex2f), GL_DYNAMIC_DRAW);
	ebo->bind_EBO();

	sdf_text_vao->link_Attribute(0, 2, GL_FLOAT, sizeof(UI_Text_Vertex2f), (void*)0);
	sdf_text_vao->link_Attribute(1, 2, GL_FLOAT, sizeof(UI_Text_Vertex2f), (void*)(2 * sizeof(float)));
	sdf_text_vao->link_Attribute(2, 4, GL_FLOAT, sizeof(UI_Text_Vertex2f), (void*)(4 * sizeof(float)));

	sdf_text_vao->unbind_VAO();
	sdf_text_vbo->unbind_VBO();
	ebo->unbind_EBO();

	ubo = std::make_unique<UBO>();
	ubo->bind_UBO(1);
	ubo_data.viewMatrix = glm::mat4(1.0f);
	ubo_data.projectionMatrix = glm::ortho(-SystemContext::screen.ratio, SystemContext::screen.ratio, -1.0f, 1.0f);
	ubo->set_data(&ubo_data, sizeof(UI_UBO), GL_DYNAMIC_DRAW);

	//reserve memory for buffers (this size should be enought for most scenarios)
	sprites_buffer.reserve(MAX_SPRITES_VERTEX_SIZE);
	sdf_text_buffer.reserve(MAX_SDF_TEXT_VERTEX_SIZE);
}

void CoreUI::UI_System::update() {
	sprites_buffer.clear();
	sdf_text_buffer.clear();
	render_queue.resize(1);
	clip_rects.clear();

	// update hit queue for interactable components
	std::vector<UI_Object*>& hit_queue = UI_BehaviourSystem::get_instance().get_hit_queue();
	uint32_t components_size = UI_ComponentManager::get_instance().size();
	if (hit_queue.capacity() < components_size) {
		hit_queue.resize(components_size);
	}
	hit_queue.clear();

	// place default clip rect based on current screen data
	clip_rects.emplace_back(0, 0, SystemContext::screen.width, SystemContext::screen.height);

	// update all dirty UI objects
	UI_ObjectManager::get_instance().update_dirty_objects();

	// replace all UI objects in render buffers
	for (auto& canvas : CanvasManager::get_instance().get_canvases()) {
		if (canvas.is_enabled) canvas.update_canvas_objects_data(render_queue, sprites_buffer, sdf_text_buffer, hit_queue);
	}

	// update interactable components
	UI_BehaviourSystem::get_instance().update();

	if (SystemContext::keyBoard.key_is_pressed(Key::KeyI)) {
		for (auto& c : hit_queue) {
			std::cout << "[Render hit queue] ID = " << c->object_id << std::endl;
		}
	}

	sprites_vbo->update_data(sprites_buffer.data(), sprites_buffer.size() * sizeof(UI_Vertex2f));
	sdf_text_vbo->update_data(sdf_text_buffer.data(), sdf_text_buffer.size() * sizeof(UI_Text_Vertex2f));
}

void CoreUI::UI_System::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	sprites_INDEX_OFFSET = 0;
	sdf_text_INDEX_OFFSET = 0;

	renderer->useScissorTest(true);
	renderer->setScissorRect(0, 0, SystemContext::screen.width, SystemContext::screen.height);

	for (auto& render_entry : render_queue) {
		//change clip rectangle
		if (render_entry.change_clip_rect) {
			ClipRectangle& rect = clip_rects[render_entry.clip_rect_id];
			renderer->setScissorRect(rect.x, rect.y, rect.w, rect.h);
		}
		//render sprites
		if (render_entry.render_type == UI_Render_Type::UI_SPRITE) {
			renderer->renderIndexedData(sprites_shader, sprites_vao, sprites_vbo, ebo, render_entry.index_count, sprites_INDEX_OFFSET);
			sprites_INDEX_OFFSET += render_entry.index_count;
		}
		//render sdf text
		else if (render_entry.render_type == UI_Render_Type::UI_TEXT) {
			renderer->renderIndexedData(sdf_text_shader, sdf_text_vao, sdf_text_vbo, ebo, render_entry.index_count, sdf_text_INDEX_OFFSET);
			sdf_text_INDEX_OFFSET += render_entry.index_count;
		}
	}
	renderer->useScissorTest(false);
}

uint16_t CoreUI::UI_System::add_clip_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	clip_rects.emplace_back(x, y, w, h);
	return clip_rects.size() - 1;
}

/*
void UI_Renderer::init_basic_inventory_slots_data() {
	float Xpos;
	float Ypos = 0.83f;
	float width = 0.1f, height = 0.1f;
	CoreResource::Sprite* sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:BasicSlot").value());

	auto add_slot_data_to_buffer = [&](uint32_t slot_type_id, std::vector<UI_Vertex2f>& buffer, float width, float height) {
		buffer.emplace_back(Xpos, Ypos,						sprite->U0, sprite->V0,							sprite->texture_id);
		buffer.emplace_back(Xpos, Ypos + height,			sprite->U0, sprite->V0 + sprite->H,				sprite->texture_id);
		buffer.emplace_back(Xpos + width, Ypos + height,	sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
		buffer.emplace_back(Xpos + width, Ypos,				sprite->U0 + sprite->W, sprite->V0,				sprite->texture_id);
		if      (slot_type_id == 0)
			main_player_ptr->inventory.add_basic_slot_bounds(Xpos, Xpos + width, Ypos, Ypos + height);
		else if (slot_type_id == 1)
			main_player_ptr->inventory.add_chest_slot_bounds(Xpos, Xpos + width, Ypos, Ypos + height);
		else if (slot_type_id == 2)
			main_player_ptr->inventory.add_helper_slot_bounds(Xpos, Xpos + width, Ypos, Ypos + height);
		else if (slot_type_id == 3)
			main_player_ptr->inventory.add_craft_info_slot_bounds(Xpos, Xpos + width, Ypos, Ypos + height);
	};

	//main 50 slots
	for (int i = 0; i < 5; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f;
		for (int j = 0; j < 10; j++) {
			add_slot_data_to_buffer(0, basic_slot_buffer, width, height);
			Xpos += 0.11f;
		}
		Ypos -= 0.11f;
	}

	//8 slots (4 for coins and ammo)
	Xpos = -SystemContext::screen.ratio * 0.97f + 10.f * 0.11f;
	for (int i = 0; i < 2; i++) {
		Ypos = 0.72f;
		for (int j = 0; j < 4; j++) {
			add_slot_data_to_buffer(0, basic_slot_buffer, width, height);
			Ypos -= 0.11f;
		}
		Xpos += 0.11f;
	}

	//16 slots for armor and accessories
	Xpos = SystemContext::screen.ratio * 0.82f;
	for (int i = 0; i < 2; i++) {
		Ypos = 0.72f;
		for (int j = 0; j < 8; j++) {
			add_slot_data_to_buffer(0, basic_slot_buffer, width, height);
			Ypos -= 0.11f;
		}
		Xpos += 0.11f;
	}

	//40 slots for chests (used with inventory when any chest is open by the player)
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:ChestSlot").value());
	Ypos = 0.83f - 0.11 * 5.f;
	for (int i = 0; i < 4; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f;
		for (int j = 0; j < 10; j++) {
			add_slot_data_to_buffer(1, chest_slot_buffer, width, height);
			Xpos += 0.11f;
		}
		Ypos -= 0.11f;
	}
	slots_vbo->update_data(basic_slot_buffer.data(), sizeof(UI_Vertex2f) * basic_slot_buffer.size());
	slots_vbo->update_data(chest_slot_buffer.data(), chest_slot_buffer.size() * sizeof(UI_Vertex2f), basic_slot_buffer.size() * sizeof(UI_Vertex2f));

	//15 slots for craft info slots
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:CraftSlot").value());
	Ypos = 0.83f - 0.11 * 10.f + 0.005f - 0.14f * 2.f;
	for (int i = 0; i < 3; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f + 0.2f;
		for (int j = 0; j < 8; j++) {
			add_slot_data_to_buffer(3, craft_info_slot_buffer, width, height);
			Xpos += 0.11f;
		}
		Ypos -= 0.11f;
	}

	//20 X 8 slots for helper crafting slots
	width = height = 0.08f;
	Ypos = 0.83f - 0.11 * 9.f + 0.02;
	for (int i = 0; i < 8; i++) {
		Xpos = -SystemContext::screen.ratio * 0.97f + 10.f * 0.11f;
		for (int j = 0; j < 20; j++) {
			add_slot_data_to_buffer(2, helper_craft_slot_buffer, width, height);
			Xpos += 0.09f;
		}
		Ypos -= 0.09f;
	}

	//craft borders
	Xpos = -SystemContext::screen.ratio * 0.97f - 0.03f;
	Ypos = 0.83f - 0.11 * 10.f + 0.125f;
	width = 0.28f; height = 0.07f;
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:CraftBorder").value());
	add_slot_data_to_buffer(4, craft_borders_buffer, width, height);
	Ypos -= 0.14f * 5.5f;
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:CraftBorder2").value());
	add_slot_data_to_buffer(4, craft_borders_buffer, width, height);
	craft_slots_vbo->update_data(craft_borders_buffer.data(), craft_borders_buffer.size() * sizeof(UI_Vertex2f));
}
void UI_Renderer::init_icons_base_vertices() {
	float Xpos;
	float Ypos = 0.9f;
	float icon_size = 0.06f;
	CoreResource::Sprite* sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:HeartFull").value());

	auto add_icon_to_buffer = [&]() {
		icons_buffer.emplace_back(Xpos, Ypos,							sprite->U0, sprite->V0,							sprite->texture_id);
		icons_buffer.emplace_back(Xpos, Ypos + icon_size,				sprite->U0, sprite->V0 + sprite->H,				sprite->texture_id);
		icons_buffer.emplace_back(Xpos + icon_size, Ypos + icon_size,	sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
		icons_buffer.emplace_back(Xpos + icon_size, Ypos,				sprite->U0 + sprite->W, sprite->V0,				sprite->texture_id);
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
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(CoreResource::SpriteManager::get_instance().get_sprite_id("Sprite:Core:ManaFull").value());
	for (int i = 0; i < 10; i++) {
		add_icon_to_buffer();
		Ypos -= 0.061f;
	}
}
void UI_Renderer::update_tooltip_data() {
	TooltipData& tooltip_data = main_player_ptr->inventory.tooltipData;
	CoreObject::ObjectInfo* item_info = CoreObject::ObjectManager::get_instance().get_object_info(tooltip_data.item_id);
	float x0 = 0.f, y0 = 0.f, x1 = 0.f, y1 = 0.f;
	float inner_height = tooltip_frame_size; //y offset
	float inner_width = 0.0f;
	glm::vec2 text_pos(tooltip_frame_size, tooltip_frame_size * 2.f); //positions of text are going from top to bottom

	auto add_info_text = [&](std::string_view text, uint32_t text_size, glm::vec4 color = glm::vec4(1.0f)) {
		float text_ortho_length = place_text_to_buffer(tooltip_text_buffer, text, text_size, text_pos.x, text_pos.y, tooltip_text_height, color);
		inner_width = text_ortho_length > inner_width ? text_ortho_length : inner_width;
		tooltip_text_INDEX_SIZE += text_size * 6;
		text_pos.y -= tooltip_text_height * 1.1f;
	};

	//update tooltip text
	tooltip_text_buffer.clear();
	tooltip_text_INDEX_SIZE = 0;
	switch (item_info->objectType) {
	case CoreObject::ObjectType::isBlock:
		text_pos.y += tooltip_text_height * 1.1f;
		inner_height += tooltip_text_height * 1.1f * 2;
		add_info_text(item_info->name, item_info->name.size());
		add_info_text("Can be placed", 13);
		break;
	case CoreObject::ObjectType::isMultiBlock:
		text_pos.y += tooltip_text_height * 1.1f;
		inner_height += tooltip_text_height * 1.1f * 2;
		add_info_text(item_info->name, item_info->name.size());
		add_info_text("Can be placed", 13);
		break;
	case CoreObject::ObjectType::isWeapon:
		text_pos.y += tooltip_text_height * 1.1f * 2;
		inner_height += tooltip_text_height * 1.1f * 3;
		add_info_text(item_info->name, item_info->name.size(), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text_builder.add_int(static_cast<CoreObject::WeaponInfo*>(item_info)->damage).add_text(" damage");
		add_info_text(text_builder.data(), text_builder.size());
		text_builder.reset();
		text_builder.add_int(static_cast<CoreObject::WeaponInfo*>(item_info)->crit_chance).add_text("% crit chance");
		add_info_text(text_builder.data(), text_builder.size());
		text_builder.reset();
		break;
	default:
		inner_height += tooltip_text_height * 1.1f;
		add_info_text(item_info->name, item_info->name.size());
		break;
	}
	inner_width += tooltip_frame_size; //x offset

	float start_x = SystemContext::mouse.ortho_x_pos + 0.03f;
	if (start_x + inner_width + tooltip_frame_size * 2.f > SystemContext::screen.ratio)
		start_x = SystemContext::mouse.ortho_x_pos - 0.01f - inner_width - tooltip_frame_size * 2.f;
	float start_y = SystemContext::mouse.ortho_y_pos - inner_height;
	adjust_tooltip_text_pos(start_x, start_y);
	text_vbo->update_data(tooltip_text_buffer.data(), tooltip_text_buffer.size() * sizeof(UI_Text_Vertex2f), TOTAL_TEXT_VERTEX_SIZE * sizeof(UI_Text_Vertex2f));

	//update tooltip background
	tooltip_buffer.clear();
	int first_sprite_id = tooltip_first_sprite_id;
	CoreResource::Sprite* sprite = nullptr;

	auto add_tooltip_part_to_buffer = [&]() {
		tooltip_buffer.emplace_back(x0, y0,		sprite->U0, sprite->V0,							sprite->texture_id);
		tooltip_buffer.emplace_back(x0, y1,		sprite->U0, sprite->V0 + sprite->H,				sprite->texture_id);
		tooltip_buffer.emplace_back(x1, y1,		sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
		tooltip_buffer.emplace_back(x1, y0,		sprite->U0 + sprite->W, sprite->V0,				sprite->texture_id);
	};
	//left bottom
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	x0 = start_x; x1 = x0 + tooltip_frame_size;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//left center
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//left top
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//center bottom
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	x0 = x1; x1 += inner_width;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//center
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//center top
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//right bottom
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	x0 = x1; x1 += tooltip_frame_size;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//right center
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//right top
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();

	slots_vbo->update_data(tooltip_buffer.data(), tooltip_buffer.size() * sizeof(UI_Vertex2f), slots_VERTEX_SIZE * sizeof(UI_Vertex2f));
}
void UI_Renderer::update_cursor_item() {
	float item_size = 0.075f;
	float start_x = SystemContext::mouse.ortho_x_pos - item_size;
	float start_y = SystemContext::mouse.ortho_y_pos - item_size * 0.5f;
	InventorySlot& cursor_slot = main_player_ptr->inventory.cursor_item;

	CoreObject::ObjectInfo* obj_info = CoreObject::ObjectManager::get_instance().get_object_info(cursor_slot.item_id);
	CoreResource::Sprite* sprite = &CoreResource::SpriteManager::get_instance().get_sprite(obj_info->sprites[0]);

	cursor_text_INDEX_SIZE = 0;
	cursor_item_text_buffer.clear();
	uint32_t text_size = 0;
	if (cursor_slot.amount > 1) {
		text_builder.add_int(cursor_slot.amount);
		text_size = text_builder.size();
		cursor_text_INDEX_SIZE = text_size * 6;
		place_text_to_buffer(cursor_item_text_buffer, text_builder.data(), text_builder.size(), start_x - 0.0175f, start_y - 0.0125f, 0.035f);
		text_builder.reset();
		text_vbo->update_data(
			cursor_item_text_buffer.data(), cursor_item_text_buffer.size() * sizeof(UI_Text_Vertex2f), TOTAL_TEXT_VERTEX_SIZE * sizeof(UI_Text_Vertex2f));
	}

	cursor_item_buffer.clear();
	float sizeX = item_size, sizeY = item_size;
	glm::vec2& size_ratio = sprite->ratio;
	start_x += (1.0f - size_ratio.x) * item_size * 0.5f;
	start_y += (1.0f - size_ratio.y) * item_size * 0.5f;
	sizeX *= size_ratio.x;
	sizeY *= size_ratio.y;
	cursor_item_buffer.emplace_back(start_x, start_y,					sprite->U0, sprite->V0,							sprite->texture_id);
	cursor_item_buffer.emplace_back(start_x, start_y + sizeY,			sprite->U0, sprite->V0 + sprite->H,				sprite->texture_id);
	cursor_item_buffer.emplace_back(start_x + sizeX, start_y + sizeY,	sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
	cursor_item_buffer.emplace_back(start_x + sizeX, start_y,			sprite->U0 + sprite->W, sprite->V0,				sprite->texture_id);
	items_vbo->update_data(cursor_item_buffer.data(), cursor_item_buffer.size() * sizeof(UI_Vertex2f), TOTAL_ITEMS_VERTEX_SIZE * sizeof(UI_Vertex2f));
}
void UI_Renderer::update_hotbar_active_slot() {
	active_hotbar_slot_buffer.clear();
	float y = 0.825f;
	float x = -SystemContext::screen.ratio * 0.97f + 0.11f * main_player_ptr->inventory.current_active_hotbar_slot - 0.005f;
	float slot_size = 0.11f;
	CoreResource::Sprite* sprite = &CoreResource::SpriteManager::get_instance().get_sprite(hotbar_frame_sprite_id);
	active_hotbar_slot_buffer.emplace_back(x, y, sprite->U0, sprite->V0, sprite->texture_id);
	active_hotbar_slot_buffer.emplace_back(x, y + slot_size, sprite->U0, sprite->V0 + sprite->H, sprite->texture_id);
	active_hotbar_slot_buffer.emplace_back(x + slot_size, y + slot_size, sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
	active_hotbar_slot_buffer.emplace_back(x + slot_size, y, sprite->U0 + sprite->W, sprite->V0, sprite->texture_id);

	slots_vbo->update_data(active_hotbar_slot_buffer.data(), active_hotbar_slot_buffer.size() * sizeof(UI_Vertex2f), slots_VERTEX_SIZE * sizeof(UI_Vertex2f));

	uint16_t active_item_id = main_player_ptr->inventory.get_active_item_id();
	tooltip_text_buffer.clear();
	tooltip_text_INDEX_SIZE = 0;

	if (active_item_id != 0) {
		CoreObject::ObjectInfo* item_info = CoreObject::ObjectManager::get_instance().get_object_info(active_item_id);
		place_text_to_buffer(tooltip_text_buffer, item_info->name, item_info->name.size(),
			-SystemContext::screen.ratio * 0.97f, 0.945f, tooltip_text_height);
		text_vbo->update_data(tooltip_text_buffer.data(), tooltip_text_buffer.size() * sizeof(UI_Text_Vertex2f), TOTAL_TEXT_VERTEX_SIZE * sizeof(UI_Text_Vertex2f));
		tooltip_text_INDEX_SIZE = item_info->name.size() * 6;
	}
}
float UI_Renderer::place_text_to_buffer(std::vector<UI_Text_Vertex2f>& buffer, std::string_view text, uint32_t text_size, float lb_x, float lb_y, float height, glm::vec4 color) {
	float start_x = lb_x;
	float start_y = lb_y - sdf_font_manager.mainFont.descender * height;
	float x0, x1, y0, y1;
	float x_offset = 0;

	for (uint32_t i = 0; i < text_size; i++) {
		GlyphData& glyph = sdf_font_manager.mainFont.glyphsData[(uint32_t)text[i] - 32U];
		x0 = start_x + x_offset + glyph.bearingX_normalized * height;
		x1 = x0 + glyph.width_normalized * height;
		y0 = start_y + (glyph.bearingY_normalized - glyph.height_normalized) * height;
		y1 = y0 + glyph.height_normalized * height;
		buffer.emplace_back(x0, y0, glyph.U0, glyph.V0, color);
		buffer.emplace_back(x0, y1, glyph.U0, glyph.V1, color);
		buffer.emplace_back(x1, y1, glyph.U1, glyph.V1, color);
		buffer.emplace_back(x1, y0, glyph.U1, glyph.V0, color);
		x_offset += height * glyph.advance_normalized;
	}
	return x_offset;
}
void UI_Renderer::adjust_tooltip_text_pos(float dX, float dY) {
	int size = tooltip_text_buffer.size() / 4;
	UI_Text_Vertex2f* ptr = tooltip_text_buffer.data();
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
		CoreResource::Sprite* sprite;
		float item_size = 0.075f;
		float x, y;
		uint32_t text_size;
		bool should_update_buffer = false;

		auto add_item_to_buffer = [&](std::vector<UI_Vertex2f>& buffer, CoreObject::ObjectInfo* item_info) {
			float sizeX = item_size, sizeY = item_size;
			glm::vec2& size_ratio = sprite->ratio;
			x += (1.0f - size_ratio.x) * item_size * 0.5f;
			y += (1.0f - size_ratio.y) * item_size * 0.5f;
			sizeX *= size_ratio.x;
			sizeY *= size_ratio.y;
			buffer.emplace_back(x, y, sprite->U0, sprite->V0, sprite->texture_id);
			buffer.emplace_back(x, y + sizeY, sprite->U0, sprite->V0 + sprite->H, sprite->texture_id);
			buffer.emplace_back(x + sizeX, y + sizeY, sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
			buffer.emplace_back(x + sizeX, y, sprite->U0 + sprite->W, sprite->V0, sprite->texture_id);
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

				CoreObject::ObjectInfo* obj_info = CoreObject::ObjectManager::get_instance().get_object_info(slot.item_id);
				sprite = &CoreResource::SpriteManager::get_instance().get_sprite(obj_info->sprites[0]);

				glm::vec4& slot_borders = main_player_ptr->inventory.slots_bounds[i];
				x = slot_borders.x + 0.0125f;
				y = slot_borders.z + 0.0125f;
				add_item_to_buffer(basic_items_buffer, obj_info);

				if (slot.amount > 1) {
					text_builder.add_int((int)slot.amount);
					text_size = text_builder.size();
					place_text_to_buffer(basic_text_buffer, text_builder.data(), text_size, slot_borders.x - 0.005f, slot_borders.z, 0.035);
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

				CoreObject::ObjectInfo* obj_info = CoreObject::ObjectManager::get_instance().get_object_info(slot.item_id);
				sprite = &CoreResource::SpriteManager::get_instance().get_sprite(obj_info->sprites[0]);

				glm::vec4& slot_borders = main_player_ptr->inventory.chest_slots_bounds[i];
				x = slot_borders.x + 0.0125f;
				y = slot_borders.z + 0.0125f;
				add_item_to_buffer(chest_items_buffer, obj_info);

				if (slot.amount > 1) {
					text_builder.add_int((int)slot.amount);
					text_size = text_builder.size();
					place_text_to_buffer(chest_text_buffer, text_builder.data(), text_size, slot_borders.x - 0.005f, slot_borders.z, 0.035);
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
	text_vbo->update_data(basic_text_buffer.data(), text_VERTEX_SIZE * sizeof(UI_Text_Vertex2f));
	//add chest items sprites and text
	if (main_player_ptr->inventory.has_open_chest) {
		TOTAL_ITEMS_INDEX_SIZE += chest_items_INDEX_SIZE;
		TOTAL_TEXT_INDEX_SIZE += chest_text_INDEX_SIZE;
		TOTAL_ITEMS_VERTEX_SIZE += chest_items_VERTEX_SIZE;
		TOTAL_TEXT_VERTEX_SIZE += chest_text_VERTEX_SIZE;
		items_vbo->update_data(chest_items_buffer.data(), chest_items_VERTEX_SIZE * sizeof(UI_Vertex2f), items_VERTEX_SIZE * sizeof(UI_Vertex2f));
		text_vbo->update_data(chest_text_buffer.data(), chest_text_VERTEX_SIZE * sizeof(UI_Text_Vertex2f), text_VERTEX_SIZE * sizeof(UI_Text_Vertex2f));
	}
}
void UI_Renderer::update_craft_slots() {
	if (main_player_ptr->inventory.current_crafts_available == 0) return;

	float Xpos, Ypos, slot_size;
	CoreResource::Sprite* sprite = &CoreResource::SpriteManager::get_instance().get_sprite(4);
	auto add_slot_to_buffer = [&]() {
		craft_slot_buffer.emplace_back(Xpos, Ypos, sprite->U0, sprite->V0, sprite->texture_id);
		craft_slot_buffer.emplace_back(Xpos, Ypos + slot_size, sprite->U0, sprite->V0 + sprite->H, sprite->texture_id);
		craft_slot_buffer.emplace_back(Xpos + slot_size, Ypos + slot_size, sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
		craft_slot_buffer.emplace_back(Xpos + slot_size, Ypos, sprite->U0 + sprite->W, sprite->V0, sprite->texture_id);
	};
	auto add_item_to_buffer = [&](std::vector<UI_Vertex2f>& buffer, CoreObject::ObjectInfo* item_info) {
		float sizeX = slot_size, sizeY = slot_size;
		glm::vec2& size_ratio = sprite->ratio;
		Xpos += (1.0f - size_ratio.x) * slot_size * 0.5f;
		Ypos += (1.0f - size_ratio.y) * slot_size * 0.5f;
		sizeX *= size_ratio.x;
		sizeY *= size_ratio.y;
		buffer.emplace_back(Xpos, Ypos, sprite->U0, sprite->V0, sprite->texture_id);
		buffer.emplace_back(Xpos, Ypos + sizeY, sprite->U0, sprite->V0 + sprite->H, sprite->texture_id);
		buffer.emplace_back(Xpos + sizeX, Ypos + sizeY, sprite->U0 + sprite->W, sprite->V0 + sprite->H, sprite->texture_id);
		buffer.emplace_back(Xpos + sizeX, Ypos, sprite->U0 + sprite->W, sprite->V0, sprite->texture_id);
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
		sprite = &CoreResource::SpriteManager::get_instance().get_sprite(craft_slot_sprite_id);
		add_slot_to_buffer();

		if (slot.item_data.amount > 1) {
			text_builder.add_int((int)slot.item_data.amount);
			text_size = text_builder.size();
			place_text_to_buffer(craft_text_buffer, text_builder.data(), text_size, Xpos, Ypos, slot_size * 0.35f);
			text_builder.reset();
			craft_text_INDEX_SIZE += 6 * text_size;
		}

		Xpos += slot_size * 0.125f;
		Ypos += slot_size * 0.125f;
		slot_size *= 0.75f;

		if (slot.item_data.item_id > 1) {
			CoreObject::ObjectInfo* item_info = CoreObject::ObjectManager::get_instance().get_object_info(slot.item_data.item_id);
			sprite = &CoreResource::SpriteManager::get_instance().get_sprite(item_info->sprites[0]);
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
			CoreObject::ObjectInfo* item_info = CoreObject::ObjectManager::get_instance().get_object_info(craft_pair.item_id);
			sprite = &CoreResource::SpriteManager::get_instance().get_sprite(item_info->sprites[0]);

			if (craft_pair.amount > 1) {
				text_builder.add_int((int)craft_pair.amount);
				text_size = text_builder.size();
				place_text_to_buffer(craft_text_buffer, text_builder.data(), text_size, Xpos, Ypos, slot_size * 0.35f);
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
			CoreObject::ObjectInfo* item_info = CoreObject::ObjectManager::get_instance().get_object_info(main_player_ptr->inventory.get_craft_item(i).item_id);
			sprite = &CoreResource::SpriteManager::get_instance().get_sprite(item_info->sprites[0]);
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

	craft_text_vbo->update_data(craft_text_buffer.data(), craft_text_buffer.size() * sizeof(UI_Text_Vertex2f));
}
void UI_Renderer::update_icons() {
	icons_INDEX_SIZE = 6 * 30;
	icons_vbo->update_data(icons_buffer.data(), icons_buffer.size() * sizeof(UI_Vertex2f));
}
*/
