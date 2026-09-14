#pragma once
#include <Resources/ResourceData.h>

namespace CoreObject {
	class ObjectComponent {
	public:
		virtual ~ObjectComponent() {}
	};

	class MultiBlockTileComponent : public ObjectComponent {
	public:
		MultiBlockTileComponent(uint16_t column, uint16_t line) :column{ column }, line{ line } {}
		uint16_t column, line;
	};

	class ChestComponent : public ObjectComponent {
	public:
		InventorySlot chest_slots[40];
	};

	class DoorComponent : public ObjectComponent {
	public:
		DoorComponent(uint8_t state) : door_state{ state } {}
		uint8_t door_state = 0; //0 - closed, 1 - opened to left, 2 - opened to right
	};
}