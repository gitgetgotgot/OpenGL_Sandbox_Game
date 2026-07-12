#include <Objects/Inventory.h>
#include <IOSystem/SystemContext.h>

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
			else if (current_active_hotbar_slot > 0)
				current_active_hotbar_slot--;
		}
		return;
	}

	tooltip_is_visible = false;
	for (int i = 0; i < INVENTORY_SIZE; i++) {
		glm::vec4& bounds = inventory_slots_bounds[i];
		if (
			SystemContext::mouse.ortho_x_pos > bounds.x && SystemContext::mouse.ortho_x_pos < bounds.y &&
			SystemContext::mouse.ortho_y_pos > bounds.z && SystemContext::mouse.ortho_y_pos < bounds.w && items[i].item_id
			) {
			tooltip_is_visible = true;
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