#pragma once
#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <Utility/DynamicArray.h>

namespace Engine::Resource {
	struct CraftingPair {
		CraftingPair() {}
		CraftingPair(uint16_t item_id, uint16_t amount) :
			item_id{ item_id }, amount{ amount } {
		}
		uint16_t item_id = 0, amount = 0;
	};

	struct CraftableItem {
		enum CraftTagsUsage : uint8_t { USAGE_ANY, USAGE_ALL };
		CraftableItem() {}
		CraftableItem(
			uint16_t item_id, uint16_t amount, Engine::Util::DynamicArray<uint16_t> craft_tags,
			CraftTagsUsage tags_usage, Engine::Util::DynamicArray<CraftingPair> items_needed
		) : item_id(item_id), item_amount(amount), craft_tags(std::move(craft_tags)),
			tags_usage{ tags_usage }, items_needed(std::move(items_needed)) {
		}
		uint16_t item_id = 0;
		uint16_t item_amount = 0; //amount per one craft
		Engine::Util::DynamicArray<uint16_t> craft_tags;
		CraftTagsUsage tags_usage = CraftTagsUsage::USAGE_ANY;
		// dynamic array max size is 256, which is more than enough for any craft realistically
		Engine::Util::DynamicArray<CraftingPair> items_needed;
	};

	struct ItemRelatedCrafts {
		std::unordered_map<uint16_t/*tag_id*/, std::vector<uint16_t>/*craftable item id*/> crafts;
	};

	class CraftManager {
	public:
		static CraftManager& Instance() {
			static CraftManager c;
			return c;
		}
		void ClearData();
		void add_craft(
			uint16_t item_id, uint16_t amount, Engine::Util::DynamicArray<uint16_t> craft_tags,
			CraftableItem::CraftTagsUsage tags_usage, Engine::Util::DynamicArray<CraftingPair> items_needed
		);
		CraftableItem& get_craft(uint16_t craft_id);
		ItemRelatedCrafts* get_item_related_crafts(uint16_t item_id);
	private:
		CraftManager() {}
		~CraftManager() {}
		std::vector<CraftableItem> crafts;
		std::unordered_map<uint16_t/*item id*/, ItemRelatedCrafts> items_related_crafts;
	};
}