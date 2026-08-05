#include <Objects/Inventory.h>
#include <IOSystem/SystemContext.h>
#include <Utility/TimeManager.h>
#include <iostream>

void Inventory::init() {
	crafting_system_ptr = CraftingSystem::get_instance();

	slots_bounds.reserve(INVENTORY_SIZE);
	chest_slots_bounds.reserve(INVENTORY_CHEST_SIZE);

	craft_slots_bounds.reserve(1); //just one main slot in the middle that is used for crafting
	craft_info_slots_bounds.reserve(24); //max available slots amount is 24
	craft_helper_slots_bounds.reserve(160); //max available helper slots amount is 160

	should_update_base_items = true;

	craft_slots_start_y = 0.83f - 0.11 * 10.f + 0.055f;
	craft_slots_scissor_rect_y0 = craft_slots_start_y - 0.14f * 4.5f;
	craft_slots_scissor_rect_height = craft_slots_start_y + 0.07f - craft_slots_scissor_rect_y0;

	last_craft_slot_up_y = craft_slots_start_y + 0.14f;
	last_craft_slot_bottom_y = craft_slots_start_y - 0.14f * 5.f;

	//init slots initial pos
	float Xpos = -SystemContext::screen.ratio * 0.97f + 0.11f, Ypos = craft_slots_start_y;
	for (int i = 0; i < 5; i++) {
		DynamicSlot& slot = craft_slots[i];
		slot.current_pos = glm::vec2(Xpos, Ypos);
		slot.current_size = 0.1f;
		Ypos -= 0.14f;
	}
	craft_slots[2].current_size = 0.15f;

	glm::vec2& main_slot_pos = craft_slots[2].current_pos;
	add_craft_slot_bounds(main_slot_pos.x - 0.075f, main_slot_pos.x + 0.075f, main_slot_pos.y - 0.075f, main_slot_pos.y + 0.075f);
}

void Inventory::toggle_inventory() {
	is_open = !is_open;
	if (!is_open) {
		crafting_with_lb_hold = false;
		has_open_chest = false;
		tooltip_is_visible = false;
		active_chest_items_ptr = nullptr;
	}
}

void Inventory::update() {
	if (current_player_flags != prev_player_flags) {
		should_update_available_crafts = true;
		prev_player_flags = current_player_flags;
	}

	if (!is_open) {
		if (SystemContext::mouse.wheel_offset != 0.0f) {
			if (SystemContext::mouse.wheel_offset < 0 && current_active_hotbar_slot < hotbar_max_slot)
				current_active_hotbar_slot++;
			else if (SystemContext::mouse.wheel_offset > 0 && current_active_hotbar_slot > 0)
				current_active_hotbar_slot--;
		}
		for (uint16_t i = Key::Key1; i <= Key::Key9; i++) {
			if (SystemContext::keyBoard.key_is_pressed(i)) {
				current_active_hotbar_slot = i - 49; break;
			}
		}
		if (SystemContext::keyBoard.key_is_pressed(Key::Key0)) current_active_hotbar_slot = 9;
		return;
	}

	if (SystemContext::keyBoard.key_is_pressed(Key::KeyC)) {
		helper_slots_are_visible = !helper_slots_are_visible;
	}

	tooltip_is_visible = false;
	can_scroll_craft_slots = false;

	if(crafting_with_lb_hold)
		craft_lb_hold_time += TimeManager::deltaTime;

	//update input and available crafts if needed
	update_inventory_input();
	update_chest_input();
	update_main_craft_slot_input();
	update_info_slots_input();
	update_helper_slots_input();

	if (should_update_available_crafts)
		update_crafts();
	//update dynamic craft slots
	update_dynamic_craft_slots();
}

void Inventory::add_basic_slot_bounds(float x0, float x1, float y0, float y1) {
	slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
}

void Inventory::add_chest_slot_bounds(float x0, float x1, float y0, float y1) {
	chest_slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
}

void Inventory::add_helper_slot_bounds(float x0, float x1, float y0, float y1) {
	craft_helper_slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
}

void Inventory::add_craft_slot_bounds(float x0, float x1, float y0, float y1) {
	craft_slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
}

void Inventory::add_craft_info_slot_bounds(float x0, float x1, float y0, float y1) {
	craft_info_slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
}

bool Inventory::place_item(uint16_t item_id, uint16_t& amount) {
	for (int i = 0; i < INVENTORY_MAIN_SIZE; i++) {
		InventorySlot& slot = items[i];
		if (slot.item_id == 0) {
			slot.item_id = item_id;
			slot.amount = amount;
			return true;
		}
		else if (slot.item_id == item_id) {
			slot.amount += amount;
			if (slot.amount > MAX_STACK_COUNT) {
				amount = slot.amount - MAX_STACK_COUNT;
				slot.amount = 9999;
			}
			return true;
		}
	}
	return false;
}

void Inventory::craft_item() {
	CraftableItem& item = get_craft_item(current_craftable_item_index);
	if (cursor_item.item_id != item.item_id && cursor_item.item_id != 0) return;

	for (auto& needed_item : item.items_needed) {
		uint16_t amount_needed = needed_item.amount;
		for (int i = 0; i < INVENTORY_MAIN_SIZE; i++) {
			InventorySlot& slot = items[i];
			if (slot.item_id == needed_item.item_id) {
				if (amount_needed <= slot.amount) {
					slot.amount -= amount_needed;
					if (slot.amount == 0) slot.item_id = 0;
					break;
				}
				else {
					amount_needed -= slot.amount;
					slot.item_id = 0; slot.amount = 0;
				}
			}
		}
	}
	cursor_item.item_id = item.item_id;
	cursor_item.amount += item.item_amount;
	should_update_available_crafts = true;
	should_update_base_items = true;
}

void Inventory::spend_active_item() {
	InventorySlot& slot = items[current_active_hotbar_slot];
	slot.amount--;
	if (slot.amount == 0) slot.item_id = 0;
	should_update_base_items = true;
}

void Inventory::process_slot_LB_click(uint32_t slot_id, InventorySlot* slots_ptr) {
	InventorySlot& slot = slots_ptr[slot_id];
	//take item from slot if cursor is empty
	if (cursor_item.item_id == 0) {
		//stop if slot has no item
		if (slot.item_id == 0) return;

		cursor_item.item_id = slot.item_id;
		cursor_item.amount = slot.amount;
		slot.item_id = 0;
		slot.amount = 0;
	}
	//place item to slot from cursor
	else {
		if (slot.item_id == 0) {
			slot.item_id = cursor_item.item_id;
			slot.amount = cursor_item.amount;
			cursor_item.item_id = 0;
			cursor_item.amount = 0;
		}
		else {
			//cursor and slot have the same item
			if (slot.item_id == cursor_item.item_id) {
				slot.amount += cursor_item.amount;
				if (slot.amount > MAX_STACK_COUNT) {
					cursor_item.amount = slot.amount - MAX_STACK_COUNT;
					slot.amount = MAX_STACK_COUNT;
				}
				else {
					cursor_item.item_id = 0;
					cursor_item.amount = 0;
				}
			}
			//have different items
			else {
				uint16_t temp_id = slot.item_id;
				uint16_t temp_amount = slot.amount;
				slot.item_id = cursor_item.item_id;
				slot.amount = cursor_item.amount;
				cursor_item.item_id = temp_id;
				cursor_item.amount = temp_amount;
			}
		}
	}
}

void Inventory::process_slot_RB_click(uint32_t slot_id, InventorySlot* slots_ptr) {
	InventorySlot& slot = slots_ptr[slot_id];

	//take one item from slot
	if (cursor_item.item_id == 0) {
		if (slot.item_id == 0) return;

		cursor_item.item_id = slot.item_id;
		cursor_item.amount++;
		slot.amount--;
		if (slot.amount == 0)
			slot.item_id = 0;
	}
	//place one item to slot from cursor
	else if (slot.item_id == 0 || slot.item_id == cursor_item.item_id) {
		slot.item_id = cursor_item.item_id;
		slot.amount++;
		cursor_item.amount--;
		if (cursor_item.amount == 0)
			cursor_item.item_id = 0;
	}
}

void Inventory::update_inventory_input() {
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		glm::vec4& bounds = slots_bounds[i];
		if (
			SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
			SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
			)
		{
			SystemContext::mouse.overlapped_by_UI_layer = true;
			if (SystemContext::mouse.lb_is_pressed()) {
				process_slot_LB_click(i, items); should_update_base_items = true; should_update_available_crafts = true;
			}
			else if (SystemContext::mouse.rb_is_pressed()) {
				process_slot_RB_click(i, items); should_update_base_items = true; should_update_available_crafts = true;
			}
			uint16_t item_id = items[i].item_id;
			if (item_id > 0) {
				if (cursor_item.item_id == 0) {
					tooltip_is_visible = true;
					tooltipData.item_id = item_id;
				}
			}
			return;
		}
	}
}

void Inventory::update_chest_input() {
	if (SystemContext::mouse.overlapped_by_UI_layer || !has_open_chest) return;

	for (int i = 0; i < INVENTORY_CHEST_SIZE; i++) {
		glm::vec4& bounds = chest_slots_bounds[i];
		if (
			SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
			SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
			)
		{
			SystemContext::mouse.overlapped_by_UI_layer = true;
			if (SystemContext::mouse.lb_is_pressed()) {
				process_slot_LB_click(i, active_chest_items_ptr); should_update_chest_items = true;
			}
			else if (SystemContext::mouse.rb_is_pressed()) {
				process_slot_RB_click(i, active_chest_items_ptr); should_update_chest_items = true;
			}
			uint16_t item_id = active_chest_items_ptr[i].item_id;
			if (item_id > 0) {
				if (cursor_item.item_id == 0) {
					tooltip_is_visible = true;
					tooltipData.item_id = item_id;
				}
			}
			return;
		}
	}
}

void Inventory::update_helper_slots_input() {
	if (SystemContext::mouse.overlapped_by_UI_layer) return;

	if (helper_slots_are_visible && current_crafts_available > 0) {
		for (int i = 0; i < current_crafts_available; i++) {
			glm::vec4& bounds = craft_helper_slots_bounds[i];
			if (
				SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
				SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
				)
			{
				SystemContext::mouse.overlapped_by_UI_layer = true;
				if (SystemContext::mouse.lb_is_pressed()) {
					current_craftable_item_index = i;
					update_dynamic_craft_slots_items();
				}
				uint16_t item_id = crafting_system_ptr->get_available_craft(i).item_id;
				if (1 > 0) {
					if (cursor_item.item_id == 0) {
						tooltip_is_visible = true;
						tooltipData.item_id = item_id;
					}
				}
				return;
			}
		}
	}
}

void Inventory::update_info_slots_input() {
	if (SystemContext::mouse.overlapped_by_UI_layer || craft_slots_move) return;

	if (current_crafts_available > 0) {
		CraftableItem& craftable_item = get_craft_item(current_craftable_item_index);
		uint32_t info_slots_size = craftable_item.items_needed.size();
		for (int i = 0; i < info_slots_size; i++) {
			glm::vec4& bounds = craft_info_slots_bounds[i];
			if (
				SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
				SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
				)
			{
				SystemContext::mouse.overlapped_by_UI_layer = true;
				
				uint16_t item_id = craftable_item.items_needed[i].item_id;
				if (cursor_item.item_id == 0) {
					tooltip_is_visible = true;
					tooltipData.item_id = item_id;
				}
				return;
			}
		}
	}
}

void Inventory::update_main_craft_slot_input() {
	if (SystemContext::mouse.overlapped_by_UI_layer || craft_slots_move) {
		crafting_with_lb_hold = false;
		return;
	}

	if (current_crafts_available > 0) {
		CraftableItem& craftable_item = get_craft_item(current_craftable_item_index);
		glm::vec4& bounds = craft_slots_bounds[0];
		if (
			SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
			SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
			)
		{
			SystemContext::mouse.overlapped_by_UI_layer = true;

			if (SystemContext::mouse.lb_is_pressed()) {
				crafting_with_lb_hold = true;
				craft_lb_hold_time = 0.0f;
				craft_lb_hold_delta_time = 0.5f;
				craft_item();
				return;
			}
			else if (SystemContext::mouse.lb_is_held()) {
				if (craft_lb_hold_time >= craft_lb_hold_delta_time) {
					if(craft_lb_hold_delta_time >= 0.055f)
						craft_lb_hold_delta_time -= 0.05f;
					craft_lb_hold_time = 0.0f;
					craft_item();
					return;
				}
			}
			if (SystemContext::mouse.lb_is_released()) {
				crafting_with_lb_hold = false;
			}

			uint16_t item_id = craftable_item.item_id;
			if (cursor_item.item_id == 0) {
				can_scroll_craft_slots = true;
				tooltip_is_visible = true;
				tooltipData.item_id = item_id;
			}
			return;
		}
	}
	crafting_with_lb_hold = false;
}

uint16_t Inventory::get_active_item_id() const {
	return items[current_active_hotbar_slot].item_id;
}

void Inventory::open_chest(InventorySlot* chest_slots_ptr) {
	active_chest_items_ptr = chest_slots_ptr;
	has_open_chest = true;
	is_open = true;
	should_update_chest_items = true;
}

void Inventory::update_dynamic_craft_slots() {
	float wheel_delta = SystemContext::mouse.wheel_offset;

	if (craft_slots_move && current_slots_y_delta >= craft_slots_move_dist) { //stop movement and reset slots
		//stop movement
		current_slots_y_delta = 0.0f;
		craft_slots_move = false;

		//return slots back to initial pos
		float Xpos = -SystemContext::screen.ratio * 0.97f + 0.11f, Ypos = craft_slots_start_y;
		for (int i = 0; i < 5; i++) {
			DynamicSlot& slot = craft_slots[i];
			slot.current_pos = glm::vec2(Xpos, Ypos);
			slot.current_size = 0.1f;
			Ypos -= 0.14f;
		}
		craft_slots[2].current_size = 0.15f;
		update_dynamic_craft_slots_items();
	}

	if (can_scroll_craft_slots && !craft_slots_move && wheel_delta != 0.0f) { //init slots movement
		if (wheel_delta < 0) {
			if (current_craftable_item_index + 1 < current_crafts_available) {
				craft_slots_move = true;
				craft_slots_move_up = true;
				DynamicSlot& last_slot = craft_slots[5];
				last_slot.current_pos = glm::vec2(-SystemContext::screen.ratio * 0.97f + 0.11f, last_craft_slot_bottom_y);
				if (current_craftable_item_index + 3 < current_crafts_available) {
					CraftableItem& item = crafting_system_ptr->get_available_craft(current_craftable_item_index + 3);
					last_slot.item_data.item_id = item.item_id;
					last_slot.item_data.amount = item.item_amount;
				}
				else {
					last_slot.item_data.item_id = 0;
					last_slot.item_data.amount = 0;
				}
				current_craftable_item_index++;
			}
		}
		else if (wheel_delta > 0) {
			if (current_craftable_item_index - 1 >= 0) {
				craft_slots_move = true;
				craft_slots_move_up = false;
				DynamicSlot& last_slot = craft_slots[5];
				last_slot.current_pos = glm::vec2(-SystemContext::screen.ratio * 0.97f + 0.11f, last_craft_slot_up_y);
				if (current_craftable_item_index - 3 >= 0) {
					CraftableItem& item = crafting_system_ptr->get_available_craft(current_craftable_item_index - 3);
					last_slot.item_data.item_id = item.item_id;
					last_slot.item_data.amount = item.item_amount;
				}
				else {
					last_slot.item_data.item_id = 0;
					last_slot.item_data.amount = 0;
				}
				current_craftable_item_index--;
			}
		}
	}

	if (craft_slots_move) { //update movable slots
		float deltaY = craft_slots_move_speed * TimeManager::deltaTime;
		uint32_t neighbor_slot_index = craft_slots_move_up ? 3 : 1;
		current_slots_y_delta += deltaY;
		for (int i = 0; i < 6; i++) {
			DynamicSlot& slot = craft_slots[i];
			if (craft_slots_move_up) {
				slot.current_pos.y += deltaY;
			}
			else {
				slot.current_pos.y -= deltaY;
			}
		}
		craft_slots[2].current_size = 0.1f + 0.05f * ((craft_slots_move_dist - current_slots_y_delta) / craft_slots_move_dist);
		craft_slots[neighbor_slot_index].current_size = 0.15f - 0.05f * ((craft_slots_move_dist - current_slots_y_delta) / craft_slots_move_dist);
	}
}

void Inventory::update_dynamic_craft_slots_items() {
	int index_offset = -2;
	for (int i = 0; i < 5; i++) {
		DynamicSlot& slot = craft_slots[i];
		int item_index = current_craftable_item_index + index_offset;
		if (item_index < 0 || item_index >= current_crafts_available) {
			slot.item_data.item_id = 0;
			slot.item_data.amount = 0;
		}
		else {
			CraftableItem& item = crafting_system_ptr->get_available_craft(item_index);
			slot.item_data.item_id = item.item_id;
			slot.item_data.amount = item.item_amount;
		}
		index_offset++;
	}
}

void Inventory::update_crafts() {
	should_update_available_crafts = false;
	crafting_system_ptr->update_available_crafts(items, INVENTORY_MAIN_SIZE, current_player_flags);
	current_crafts_available = crafting_system_ptr->available_crafts.size();
	if (current_craftable_item_index >= current_crafts_available)
		current_craftable_item_index = current_crafts_available - 1;
	update_dynamic_craft_slots_items();
}

CraftableItem& Inventory::get_craft_item(uint32_t index) {
	return crafting_system_ptr->get_available_craft(index);
}