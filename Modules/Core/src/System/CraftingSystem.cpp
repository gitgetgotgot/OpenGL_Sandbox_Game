#include "Core/System/CraftingSystem.h"
#include "Core/System/Craft.h"
#include <unordered_map>
#include <algorithm>

void CraftSystem::update_available_crafts(
	Engine::Resource::InventorySlot* inventory_slots_ptr,
	uint16_t slots_size,
	std::vector<uint16_t>& player_tags
) {
	std::unordered_map</*item_id*/uint16_t, /*item_amount*/uint16_t> unique_items;
	unique_items.reserve(slots_size);

	available_crafts.clear();
	checked_crafts.clear();

	for (uint32_t i = 0; i < slots_size; i++) {
		Engine::Resource::InventorySlot& inv_slot = inventory_slots_ptr[i];
		unique_items[inv_slot.item_id] += inv_slot.amount; //element in unique_items is created if doesn't exist yet
	}

	auto check_specific_crafts = [&](std::vector<uint16_t>& crafts_vec) {
		for (auto& craft_id : crafts_vec) {
			if (checked_crafts.find(craft_id) != checked_crafts.end()) continue; //already checked this craft

			Engine::Core::Craft::CraftableItem& craft = Engine::Core::Craft::CraftManager::Instance().get_craft(craft_id);
			uint8_t craft_items_size = craft.items_needed.size;
			bool can_craft = true;

			// check if player has all tags needed if all tags are used
			if (craft.tags_usage == Engine::Core::Craft::CraftableItem::CraftTagsUsage::USAGE_ALL) {
				uint8_t tags_size = craft.craft_tags.size;
				for (uint8_t j = 0; j < tags_size; ++j) {
					if (!std::binary_search(player_tags.begin(), player_tags.end(), craft.craft_tags[j])) {
						can_craft = false;
						break;
					}
				}
			}

			// check if player has all items needed
			if (can_craft) {
				for (uint32_t i = 0; i < craft_items_size; i++) {
					auto& needed_item = craft.items_needed[i];
					auto it = unique_items.find(needed_item.item_id);
					if (it == unique_items.end() || it->second < needed_item.amount) {
						can_craft = false;
						break;
					}
				}
			}

			if (can_craft) available_crafts.emplace_back(craft_id);
			checked_crafts.emplace(craft_id);
		}
	};

	for (auto& unique_item : unique_items) {
		Engine::Core::Craft::ItemRelatedCrafts* item_crafts = Engine::Core::Craft::CraftManager::Instance().get_item_related_crafts(unique_item.first);
		if (item_crafts) { //if this item is used in crafts
			for (auto& tag_id : player_tags) {
				if (auto it = item_crafts->crafts.find(tag_id); it != item_crafts->crafts.end()) { //if item has crafts with this tag
					check_specific_crafts(it->second);
				}
			}
		}
	}
}