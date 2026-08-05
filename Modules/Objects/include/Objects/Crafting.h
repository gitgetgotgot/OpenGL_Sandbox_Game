#pragma once
#include <Objects/GameObjects.h>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>

enum class CraftCondition : uint32_t {
	c_NOTHING     = 0,
	c_WORKBENCH   = 1 << 0,
	c_FURNACE     = 1 << 1,
	c_ANVIL       = 1 << 2,
	c_BOTTLE      = 1 << 3,
	c_TABLE       = 1 << 4
};

struct CraftingPair {
	uint16_t item_id = 0, amount = 0;
};

struct CraftableItem {
	CraftableItem() {}
	CraftableItem(uint16_t item_id, uint16_t amount, CraftCondition condition, std::vector<CraftingPair> items_needed) :
		item_id(item_id), item_amount(amount), condition(condition), items_needed(std::move(items_needed)) {}
	uint16_t item_id = 0;
	uint16_t item_amount = 0; //amount per one craft
	std::vector<CraftingPair> items_needed;
	CraftCondition condition = CraftCondition::c_NOTHING;
};

struct ItemCrafts {
	std::vector<uint32_t> simple_crafts;
	std::vector<uint32_t> workbench_crafts;
	std::vector<uint32_t> furnace_crafts;
	std::vector<uint32_t> anvil_crafts;
	std::vector<uint32_t> bottle_crafts;
	std::vector<uint32_t> table_crafts;
};

class CraftingSystem {
public:
	static CraftingSystem* get_instance() {
		static CraftingSystem sys;
		return &sys;
	}
	void update_available_crafts(InventorySlot* inventory_slots_ptr, uint32_t slots_size, uint32_t player_flags);
	void add(uint16_t item_id, uint16_t amount, CraftCondition condition, std::vector<CraftingPair> items_needed);
	CraftableItem& get_available_craft(uint32_t index);

	std::vector<uint32_t> available_crafts;
private:
	CraftingSystem() {}
	~CraftingSystem() {}
	std::vector<CraftableItem> crafts;
	std::unordered_map<uint16_t, ItemCrafts> items_crafts;
	std::unordered_set<uint32_t> checked_crafts;
};