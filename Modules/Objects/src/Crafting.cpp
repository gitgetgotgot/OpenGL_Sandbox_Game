#include <Objects/Crafting.h>

void CraftingSystem::update_available_crafts(InventorySlot* inventory_slots_ptr, uint32_t slots_size, uint32_t player_flags) {
	std::unordered_map</*ID*/uint16_t, /*Amount*/uint16_t> unique_items;
	unique_items.reserve(slots_size);

	available_crafts.clear();
	checked_crafts.clear();

	for (uint32_t i = 0; i < slots_size; i++) {
		InventorySlot& inv_slot = inventory_slots_ptr[i];
		unique_items[inv_slot.item_id] += inv_slot.amount; //element is created it not exists
	}

	auto check_specific_crafts = [&](std::vector<uint32_t>& vec) {
		for (auto& craft_ID : vec) {
			if (checked_crafts.find(craft_ID) != checked_crafts.end()) continue; //already checked this craft

			CraftableItem& craft = crafts[craft_ID];
			uint32_t craft_items_size = craft.items_needed.size();
			bool can_craft = true;

			for (uint32_t i = 0; i < craft_items_size; i++) {
				auto& needed_item = craft.items_needed[i];
				auto it = unique_items.find(needed_item.item_id);
				if (it == unique_items.end() || it->second < needed_item.amount) {
					can_craft = false;
					break;
				}
			}

			if (can_craft) available_crafts.emplace_back(craft_ID);
			checked_crafts.emplace(craft_ID);
		}
	};

	for (auto& unique_item : unique_items) {
		auto it = items_crafts.find(unique_item.first);
		if (it != items_crafts.end()) { //if this item is used in crafts
			ItemCrafts& item_crafts = it->second;
			//check simple crafts
			check_specific_crafts(item_crafts.simple_crafts);
			if(player_flags & (uint32_t)CraftCondition::c_WORKBENCH)
				check_specific_crafts(item_crafts.workbench_crafts);
			if (player_flags & (uint32_t)CraftCondition::c_FURNACE)
				check_specific_crafts(item_crafts.furnace_crafts);
			if (player_flags & (uint32_t)CraftCondition::c_ANVIL)
				check_specific_crafts(item_crafts.anvil_crafts);
			if (player_flags & (uint32_t)CraftCondition::c_BOTTLE)
				check_specific_crafts(item_crafts.bottle_crafts);
			if (player_flags & (uint32_t)CraftCondition::c_TABLE)
				check_specific_crafts(item_crafts.table_crafts);
		}
	}
}

void CraftingSystem::add(uint16_t item_id, uint16_t amount, CraftCondition condition, std::vector<CraftingPair> items_needed) {
	crafts.emplace_back(item_id, amount, condition, std::move(items_needed));
	uint32_t craft_ID = uint32_t(crafts.size() - 1);
	CraftableItem& item = crafts.back();

	for (auto& needed_item : item.items_needed) {
		ItemCrafts& item_crafts = items_crafts[needed_item.item_id]; //element is created if not exists

		switch (item.condition) {
		case CraftCondition::c_NOTHING:
			item_crafts.simple_crafts.emplace_back(craft_ID);
			break;
		case CraftCondition::c_WORKBENCH:
			item_crafts.workbench_crafts.emplace_back(craft_ID);
			break;
		case CraftCondition::c_FURNACE:
			item_crafts.furnace_crafts.emplace_back(craft_ID);
			break;
		}
	}
}

CraftableItem& CraftingSystem::get_available_craft(uint32_t index) {
	return crafts[available_crafts[index]];
}