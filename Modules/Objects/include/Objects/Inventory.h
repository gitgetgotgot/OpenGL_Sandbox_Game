#pragma once
#include <Objects/GameObjects.h>

struct TooltipData {
	glm::vec2 inner_size{ 0.f, 0.f };
	std::string item_name;
	std::string description;
	glm::vec4 item_name_color = { 1.f, 1.f, 1.f, 1.f };
	glm::vec4 description_color = { 1.f, 1.f, 1.f, 1.f };
};

struct DynamicSlot {
	float current_size = 0.1f;
	glm::vec2 current_pos{ 0.0f };
	InventorySlot item_slot;
};

class Inventory {
public:
	void init();
	void toggle_inventory();
	void update();

	void add_inventory_basic_slot_bounds(float x0, float x1, float y0, float y1);
	void add_inventory_chest_slot_bounds(float x0, float x1, float y0, float y1);
	
	bool place_item(uint16_t item_id, uint16_t& amount);
	void process_slot_LB_click(uint32_t slot_id, InventorySlot* slots_ptr);
	void process_slot_RB_click(uint32_t slot_id, InventorySlot* slots_ptr);
	uint16_t get_active_item_id();
	void open_chest(InventorySlot* chest_slots_ptr);
	void update_dynamic_craft_slots();

	bool is_open = false;
	bool has_open_chest = false;
	bool tooltip_is_visible = false;
	bool should_update_base_items = true;
	bool should_update_chest_items = false;
	uint8_t current_active_hotbar_slot = 0;
	static const uint32_t INVENTORY_SIZE = 74;
	static const uint32_t INVENTORY_MAIN_SIZE = 50;
	static const uint32_t INVENTORY_CHEST_SIZE = 40;
	static const uint16_t MAX_STACK_COUNT = 9999;
	InventorySlot items[INVENTORY_SIZE];
	InventorySlot cursor_item;
	InventorySlot* active_chest_items_ptr = nullptr;
	TooltipData tooltipData;
	std::vector<glm::vec4> inventory_slots_bounds;
	std::vector<glm::vec4> inventory_chest_slots_bounds;
	DynamicSlot craft_slots[6];
	bool craft_slots_move = false;
	float craft_slots_scissor_rect_y0;
	float craft_slots_scissor_rect_height;
	bool helper_slots_are_visible = false;
	uint32_t current_crafts_available = 33;
private:
	//50 main, 8 for coins and ammo, 16 for armor and accessories
	static const uint8_t hotbar_max_slot = 9;
	//stores the index of the last slot where the object was taken with mouse (puts it back if leaving inventory with currently taken object)
	uint32_t index_of_last_slot_picked = 0;
	bool craft_slots_move_up = false;
	float craft_slots_move_speed = 0.42f;
	float craft_slots_move_dist = 0.14f;
	float current_slots_y_delta = 0.0f;
	float last_craft_slot_up_y;
	float last_craft_slot_bottom_y;
	float craft_slots_start_y;
	uint32_t current_craftable_item_index = 0;
};