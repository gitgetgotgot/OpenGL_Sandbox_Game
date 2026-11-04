#include "Core/System/Craft.h"
#include <Resources/TagRegistry.h>
#include <Objects/ObjectManager.h>

void Engine::Core::Craft::CraftManager::ClearData() {
	//crafts = {};
	items_related_crafts = {};
}

bool Engine::Core::Craft::CraftManager::load_crafts_from_data(
	std::filesystem::path& data_file_path,
	Engine::Util::DataParser& parser
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		if (!j.contains("result_UID"))
			throw std::runtime_error("Craft entry in " + data_file_path.string() + " has no result_UID!");
		std::string result_UID = j["result_UID"].get<std::string>();
		try {
			auto item_info_id = Engine::Object::ObjectManager::Instance().get_object_id(result_UID);
			if (!item_info_id.has_value())
				throw std::runtime_error("Craft entry in " + data_file_path.string() + ": <" + result_UID + "> doesn't exist!");

			uint16_t item_id = item_info_id.value();
			uint16_t amount = j["amount"].get<uint16_t>();

			Engine::Util::DynamicArray<uint16_t> craft_tags;
			CraftableItem::CraftTagsUsage tags_usage;
			if (j.contains("craft_tags")) {
				const auto& craft_tags_v = j["craft_tags"].get<std::vector<std::string>>();
				craft_tags = Engine::Util::DynamicArray<uint16_t>(craft_tags_v.size());
				uint8_t index = 0;
				for (auto& tag_name : craft_tags_v) {
					auto tag_id_opt = Engine::Resource::Tag::TagRegistry::Instance().get_tag(tag_name);
					if (tag_id_opt.has_value())
						craft_tags[index++] = tag_id_opt.value();
					else {
						//Engine::Resource::throw_reference_error("Craft entry with item id = " + item_id, tag_name);
					}
				}
				if (j["tags_usage"].get<std::string>() == "ANY") {
					tags_usage = CraftableItem::CraftTagsUsage::USAGE_ANY;
				}
				else {
					tags_usage = CraftableItem::CraftTagsUsage::USAGE_ALL;
				}
			}
			else {
				craft_tags = Engine::Util::DynamicArray<uint16_t>(1);
				craft_tags[0] = 0; // Tag Core:None
				tags_usage = CraftableItem::CraftTagsUsage::USAGE_ANY;
			}

			const auto& items = j["items"].get<std::vector<nlohmann::json>>();
			Engine::Util::DynamicArray<CraftingPair> craft_pairs(items.size());
			uint8_t index = 0;
			for (const auto& i : items) {
				std::string item_UID = i["item_UID"].get<std::string>();
				auto item_info_ID = Engine::Object::ObjectManager::Instance().get_object_id(item_UID);
				if (!item_info_ID.has_value()) {
					//Engine::Resource::throw_reference_error(result_UID, item_UID);
				}
				uint16_t item_ID = item_info_ID.value();
				uint16_t item_amount = i["item_amount"].get<uint16_t>();
				craft_pairs[index++] = CraftingPair((uint16_t)item_ID, item_amount);
			}
			add_craft(item_id, amount, std::move(craft_tags), tags_usage, std::move(craft_pairs));
		}
		catch (const std::exception& e) {
			//Engine::Resource::throw_resource_error(result_UID, std::string("In file ") + file_path.string() + ": " + e.what());
		}
	}
	return true;
}

void Engine::Core::Craft::CraftManager::add_craft(
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

Engine::Core::Craft::CraftableItem& Engine::Core::Craft::CraftManager::get_craft(uint16_t craft_id) {
	return crafts[craft_id];
}

Engine::Core::Craft::ItemRelatedCrafts* Engine::Core::Craft::CraftManager::get_item_related_crafts(uint16_t item_id) {
	if (auto it = items_related_crafts.find(item_id); it != items_related_crafts.end()) {
		return &items_related_crafts[item_id];
	}
	else {
		return nullptr;
	}
}