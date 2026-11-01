#pragma once
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <Resources/ResourceData.h>

class CraftSystem {
public:
	static CraftSystem& Instance() {
		static CraftSystem s;
		return s;
	}
	void update_available_crafts(
		Engine::Resource::InventorySlot* inventory_slots_ptr,
		uint16_t slots_size,
		std::vector<uint16_t>& player_tags
	);
	std::vector<uint16_t> available_crafts;
	std::unordered_set<uint16_t> checked_crafts;
private:
	CraftSystem() = default;
	~CraftSystem() = default;
};