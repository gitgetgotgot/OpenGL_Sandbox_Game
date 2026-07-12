#pragma once
#include <Objects/GameObjects.h>

class Inventory {
public:
	void init();
	void toggle_inventory();
	void update();

	void add_inventory_basic_slot_bounds(float x0, float x1, float y0, float y1);
	void add_inventory_chest_slot_bounds(float x0, float x1, float y0, float y1);
	
	bool place_item(uint16_t item_id, uint16_t& amount);

	bool is_open = false;
	bool has_open_chest = false;
	bool tooltip_is_visible = false;
	bool isDirty = true;
	uint8_t current_active_hotbar_slot = 0;
	static const uint32_t INVENTORY_SIZE = 74;
	static const uint32_t INVENTORY_MAIN_SIZE = 50;
	static const uint32_t INVENTORY_CHEST_SIZE = 40;
	static const uint16_t MAX_STACK_COUNT = 9999;
	InventorySlot items[INVENTORY_SIZE];
	InventorySlot cursor_object;
private:
	//50 main, 8 for coins and ammo, 16 for armor and accessories
	static const uint8_t hotbar_max_slot = 9;
	//stores the index of the last slot where the object was taken with mouse (puts it back if leaving inventory with currently taken object)
	uint32_t index_of_last_slot_picked = 0;

	std::vector<glm::vec4> inventory_slots_bounds;
	std::vector<glm::vec4> inventory_chest_slots_bounds;
};