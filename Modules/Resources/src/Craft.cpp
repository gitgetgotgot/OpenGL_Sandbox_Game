#include "Resources/Craft.h"

void Engine::Resource::CraftManager::ClearData() {
	//crafts = {};
	items_related_crafts = {};
}

void Engine::Resource::CraftManager::add_craft(
	uint16_t item_id, uint16_t amount, Engine::Util::DynamicArray<uint16_t> craft_tags,
	CraftableItem::CraftTagsUsage tags_usage, Engine::Util::DynamicArray<CraftingPair> items_needed
) {
	crafts.emplace_back(item_id, amount, std::move(craft_tags), tags_usage, std::move(items_needed));
	uint16_t craft_id = uint16_t(crafts.size() - 1);
	CraftableItem& item = crafts.back();

	uint8_t size = item.items_needed.size;
	for (uint8_t i = 0; i < size; ++i) {
		CraftingPair craft = item.items_needed[i];
		ItemRelatedCrafts& item_related_crafts = items_related_crafts[craft.item_id]; //element is created if doesn't exist

		if (tags_usage == CraftableItem::CraftTagsUsage::USAGE_ANY) {
			// if any tag is used, then place this craft_id to all of them
			uint8_t tags_size = item.craft_tags.size;
			for (uint8_t j = 0; j < tags_size; ++j) {
				item_related_crafts.crafts[item.craft_tags[j]].emplace_back(craft_id);
			}
		}
		else {
			// else if all tags are used, then place just the first 
			// if player doesn't have first tag ---> ha can't craft this item,
			// if player has this tag, then he can check if he can craft this item, having all tags and items needed
			item_related_crafts.crafts[item.craft_tags[0]].emplace_back(craft_id);
		}
	}
}

Engine::Resource::CraftableItem& Engine::Resource::CraftManager::get_craft(uint16_t craft_id) {
	return crafts[craft_id];
}

Engine::Resource::ItemRelatedCrafts* Engine::Resource::CraftManager::get_item_related_crafts(uint16_t item_id) {
	if (auto it = items_related_crafts.find(item_id); it != items_related_crafts.end()) {
		return &items_related_crafts[item_id];
	}
	else {
		return nullptr;
	}
}