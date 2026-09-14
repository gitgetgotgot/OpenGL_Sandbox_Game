#pragma once
#include <cstdint>

struct InventorySlot {
	uint16_t item_id = 0;
	uint16_t amount = 0;
};

struct EffectEntry {
	uint16_t id = 0;
	float duration = 0.0f;
};

struct DropEntry {
	uint16_t id = 0;
	uint16_t amount = 0;
};

struct EntityDropEntry {
	EntityDropEntry(uint16_t id, float drop_chance, uint16_t amount) :
		id{ id }, drop_chance{ drop_chance }, amount_min{ amount }, amount_max{ amount } {}
	EntityDropEntry(uint16_t id, float drop_chance, uint16_t amount_min, uint16_t amount_max) :
		id{ id }, drop_chance{ drop_chance }, amount_min{ amount_min }, amount_max{ amount_max } {}
	uint16_t id = 0;
	uint16_t amount_min = 0;
	uint16_t amount_max = 0;
	float drop_chance = 0.f;
};