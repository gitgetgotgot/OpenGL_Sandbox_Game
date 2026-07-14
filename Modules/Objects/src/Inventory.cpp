#include <Objects/Inventory.h>
#include <IOSystem/SystemContext.h>
#include <iostream>

void Inventory::init() {
	inventory_slots_bounds.reserve(INVENTORY_SIZE);
	inventory_chest_slots_bounds.reserve(INVENTORY_CHEST_SIZE);
}

void Inventory::toggle_inventory() {
	is_open = !is_open;
	if (!is_open) has_open_chest = false;
}

void Inventory::update() {
	if (!is_open) {
		if (SystemContext::mouse.wheel_offset != 0.0f) {
			if (SystemContext::mouse.wheel_offset > 0 && current_active_hotbar_slot < hotbar_max_slot)
				current_active_hotbar_slot++;
			else if (SystemContext::mouse.wheel_offset < 0 && current_active_hotbar_slot > 0)
				current_active_hotbar_slot--;
		}
		return;
	}

	tooltip_is_visible = false;
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		glm::vec4& bounds = inventory_slots_bounds[i];
		if (
			SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
			SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w
			)
		{
			uint16_t item_id = items[i].item_id;
			if (item_id > 0) {
				if (cursor_item.item_id == 0) {
					tooltip_is_visible = true;
					tooltipData.item_name = ObjectsDB::objectInfo[item_id]->name;
				}
			}
			if (SystemContext::mouse.lb_is_pressed()) {
				process_slot_LB_click(i);
			}
			else if (SystemContext::mouse.rb_is_pressed()) {
				process_slot_RB_click(i);
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
				) {
				tooltip_is_visible = true;

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

void Inventory::process_slot_LB_click(uint32_t slot_id) {
	InventorySlot& slot = items[slot_id];
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
	should_update_base_items = true;
}

void Inventory::process_slot_RB_click(uint32_t slot_id) {

}
