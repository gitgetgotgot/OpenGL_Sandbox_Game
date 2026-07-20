#include <Objects/Inventory.h>
#include <IOSystem/SystemContext.h>
#include <Utility/TimeManager.h>
#include <iostream>

void Inventory::init() {
	inventory_slots_bounds.reserve(INVENTORY_SIZE);
	inventory_chest_slots_bounds.reserve(INVENTORY_CHEST_SIZE);

	craft_slots_start_y = 0.83f - 0.11 * 10.f + 0.055f;
	craft_slots_scissor_rect_y0 = craft_slots_start_y - 0.14f * 4.5f;
	craft_slots_scissor_rect_height = craft_slots_start_y + 0.07f - craft_slots_scissor_rect_y0;

	last_craft_slot_up_y = craft_slots_start_y + 0.14f;
	last_craft_slot_bottom_y = craft_slots_start_y - 0.14f * 5.f;

	float Xpos = -SystemContext::screen.ratio * 0.97f + 0.11f, Ypos = craft_slots_start_y;
	for (int i = 0; i < 5; i++) {
		craft_slots[i].current_pos = glm::vec2(Xpos, Ypos);
		Ypos -= 0.14f;
	}
	craft_slots[2].current_size = 0.15f;
}

void Inventory::toggle_inventory() {
	is_open = !is_open;
	if (!is_open) {
		has_open_chest = false;
		tooltip_is_visible = false;
		active_chest_items_ptr = nullptr;
	}
}

void Inventory::update() {
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

	update_dynamic_craft_slots();

	tooltip_is_visible = false;
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		glm::vec4& bounds = inventory_slots_bounds[i];
		if (
			SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
			SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
			)
		{
			SystemContext::mouse.overlapped_by_UI_layer = true;
			if (SystemContext::mouse.lb_is_pressed()) {
				process_slot_LB_click(i, items); should_update_base_items = true;
			}
			else if (SystemContext::mouse.rb_is_pressed()) {
				process_slot_RB_click(i, items); should_update_base_items = true;
			}
			uint16_t item_id = items[i].item_id;
			if (item_id > 0) {
				if (cursor_item.item_id == 0) {
					tooltip_is_visible = true;
					tooltipData.item_name = ObjectsDB::objectInfo[item_id]->name;
				}
			}
			break;
		}
	}
	if (has_open_chest) {
		for (int i = 0; i < INVENTORY_CHEST_SIZE; i++) {
			glm::vec4& bounds = inventory_chest_slots_bounds[i];
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
						tooltipData.item_name = ObjectsDB::objectInfo[item_id]->name;
					}
				}
				break;
			}
		}
	}
}

void Inventory::add_inventory_basic_slot_bounds(float x0, float x1, float y0, float y1) {
	inventory_slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
}

void Inventory::add_inventory_chest_slot_bounds(float x0, float x1, float y0, float y1) {
	inventory_chest_slots_bounds.emplace_back(glm::vec4(x0, x1, y0, y1));
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

}

uint16_t Inventory::get_active_item_id() {
	return items[current_active_hotbar_slot].item_id;
}

void Inventory::open_chest(InventorySlot* chest_slots_ptr) {
	active_chest_items_ptr = chest_slots_ptr;
	has_open_chest = true;
	is_open = true;
	should_update_chest_items = true;
}

void Inventory::update_dynamic_craft_slots() {
	if (craft_slots_move && current_slots_y_delta >= craft_slots_move_dist) {
		//stop movement
		current_slots_y_delta = 0.0f;
		craft_slots_move = false;
		float Ypos = craft_slots_start_y;
		float Xpos = -SystemContext::screen.ratio * 0.97f + 0.11f;
		for (int i = 0; i < 5; i++) {
			DynamicSlot& slot = craft_slots[i];
			slot.current_pos = glm::vec2(Xpos, Ypos);
			slot.current_size = 0.1f;
			Ypos -= 0.14f;
		}
		craft_slots[2].current_size = 0.15f;
	}

	if (craft_slots_move) {
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
	else {
		if (SystemContext::mouse.wheel_offset < 0) {
			craft_slots_move = true;
			craft_slots_move_up = true;
			DynamicSlot& last_slot = craft_slots[5];
			last_slot.current_pos = glm::vec2(-SystemContext::screen.ratio * 0.97f + 0.11f, last_craft_slot_bottom_y);
		}
		else if (SystemContext::mouse.wheel_offset > 0) {
			craft_slots_move = true;
			craft_slots_move_up = false;
			DynamicSlot& last_slot = craft_slots[5];
			last_slot.current_pos = glm::vec2(-SystemContext::screen.ratio * 0.97f + 0.11f, last_craft_slot_up_y);
		}
	}
}