#pragma once
#include <Objects/Crafting.h>

struct TooltipData {
	glm::vec2 inner_size{ 0.f, 0.f };
	uint16_t item_id;
};

struct DynamicSlot {
	float current_size = 0.1f;
	glm::vec2 current_pos{ 0.0f };
	InventorySlot item_data;
};

class Inventory {
public:
	void init();
	void toggle_inventory();
	void update();

	void add_basic_slot_bounds(float x0, float x1, float y0, float y1);
	void add_chest_slot_bounds(float x0, float x1, float y0, float y1);
	void add_helper_slot_bounds(float x0, float x1, float y0, float y1);
	void add_craft_slot_bounds(float x0, float x1, float y0, float y1);
	void add_craft_info_slot_bounds(float x0, float x1, float y0, float y1);
	
	bool place_item(uint16_t item_id, uint16_t& amount);
	void craft_item();
	void spend_active_item();
	void process_slot_LB_click(uint32_t slot_id, InventorySlot* slots_ptr);
	void process_slot_RB_click(uint32_t slot_id, InventorySlot* slots_ptr);
	void update_inventory_input();
	void update_chest_input();
	void update_helper_slots_input();
	void update_info_slots_input();
	void update_main_craft_slot_input();

	uint16_t get_active_item_id() const;
	void open_chest(InventorySlot* chest_slots_ptr);
	void update_dynamic_craft_slots();
	void update_dynamic_craft_slots_items();
	void update_crafts();
	CraftableItem& get_craft_item(uint32_t index);

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

	std::vector<glm::vec4> slots_bounds;
	std::vector<glm::vec4> chest_slots_bounds;
	std::vector<glm::vec4> craft_slots_bounds;
	std::vector<glm::vec4> craft_info_slots_bounds;
	std::vector<glm::vec4> craft_helper_slots_bounds;

	DynamicSlot craft_slots[6];
	bool craft_slots_move = false;
	float craft_slots_scissor_rect_y0;
	float craft_slots_scissor_rect_height;
	bool helper_slots_are_visible = false;
	uint32_t current_crafts_available = 160;
	int current_craftable_item_index = 0;
	CraftingSystem* crafting_system_ptr = nullptr;

	uint32_t current_player_flags = 0;
	uint32_t prev_player_flags = 0;
private:
	//50 main, 8 for coins and ammo, 16 for armor and accessories
	static const uint8_t hotbar_max_slot = 9;
	//stores the index of the last slot where the object was taken with mouse (puts it back if leaving inventory with currently taken object)
	uint32_t index_of_last_slot_picked = 0;
	bool can_scroll_craft_slots = false;
	bool should_update_available_crafts = false;

	bool craft_slots_move_up = false;
	float craft_slots_move_speed = 0.84f;
	float craft_slots_move_dist = 0.14f;
	float current_slots_y_delta = 0.0f;

	float last_craft_slot_up_y;
	float last_craft_slot_bottom_y;
	float craft_slots_start_y;

	float craft_lb_hold_time = 0.0f;
	float craft_lb_hold_delta_time = 0.5f;
	bool crafting_with_lb_hold = false;
};