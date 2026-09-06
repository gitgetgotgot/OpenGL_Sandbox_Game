#pragma once
#include <cstdint>

namespace TagID {
//Objects
	//Block
	inline uint16_t IS_SOLID_BLOCK;
	inline uint16_t IS_WOOD;
	inline uint16_t IS_TREE_TOP;
	inline uint16_t IS_PLATFORM;
	inline uint16_t IS_TORCH;
	inline uint16_t IS_LIQUID;
	inline uint16_t IS_GRASS;
	inline uint16_t IS_PLANT;
	inline uint16_t IS_GLASS;
	inline uint16_t IS_BOTTLE;

	//MultiBlock
	inline uint16_t IS_WORKBENCH;
	inline uint16_t IS_FURNACE;
	inline uint16_t IS_CHEST;
	inline uint16_t IS_DOOR;
	inline uint16_t IS_TABLE;
	inline uint16_t IS_CHAIR;
	inline uint16_t IS_ANVIL;
	inline uint16_t IS_LAMP;
	inline uint16_t IS_CHANDELIER;
	inline uint16_t IS_LIFE_CRYSTAL;

	//Item
	inline uint16_t IS_COIN;

	//Ammo
	inline uint16_t IS_ARROW;
	inline uint16_t IS_BULLET;

	//Weapon
	inline uint16_t IS_PICKAXE;
	inline uint16_t IS_SWORD;
	inline uint16_t IS_PIERCING;
	inline uint16_t IS_AXE;
	inline uint16_t IS_HAMMER;
	inline uint16_t IS_GUN;
	inline uint16_t IS_SHOTGUN;
	inline uint16_t IS_BOW;
	inline uint16_t IS_THROWABLE;
	inline uint16_t IS_SPEAR;
	inline uint16_t IS_MAGICAL;

	//Consumable
	inline uint16_t IS_POTION;
	inline uint16_t IS_FOOD;

//Entities
	inline uint16_t IS_SLIME;
}

namespace CoreTags {
	void register_core_tags();
}