#include "Resources/CoreTags.h"
#include "Resources/TagRegistry.h"

void CoreTags::register_core_tags() {
	//Block
	TagID::IS_SOLID_BLOCK =		TagRegistry::get_instance().add_tag("Core:SolidBlock");
	TagID::IS_WOOD =			TagRegistry::get_instance().add_tag("Core:Wood");
	TagID::IS_TREE_TOP =		TagRegistry::get_instance().add_tag("Core:TreeTop");
	TagID::IS_PLATFORM =		TagRegistry::get_instance().add_tag("Core:Platform");
	TagID::IS_TORCH =			TagRegistry::get_instance().add_tag("Core:Torch");
	TagID::IS_LIQUID =			TagRegistry::get_instance().add_tag("Core:Liquid"); //should probably make new Object for liquids later
	TagID::IS_GRASS =			TagRegistry::get_instance().add_tag("Core:Grass");
	TagID::IS_PLANT =			TagRegistry::get_instance().add_tag("Core:Plant");
	TagID::IS_GLASS =			TagRegistry::get_instance().add_tag("Core:Glass");
	TagID::IS_BOTTLE =			TagRegistry::get_instance().add_tag("Core:Bottle");

	//Multiblock
	TagID::IS_WORKBENCH =		TagRegistry::get_instance().add_tag("Core:Workbench");
	TagID::IS_FURNACE =			TagRegistry::get_instance().add_tag("Core:Furnace");
	TagID::IS_CHEST =			TagRegistry::get_instance().add_tag("Core:Chest");
	TagID::IS_DOOR =			TagRegistry::get_instance().add_tag("Core:Door");
	TagID::IS_TABLE =			TagRegistry::get_instance().add_tag("Core:Table");
	TagID::IS_CHAIR =			TagRegistry::get_instance().add_tag("Core:Chair");
	TagID::IS_ANVIL =			TagRegistry::get_instance().add_tag("Core:Anvil");
	TagID::IS_LAMP =			TagRegistry::get_instance().add_tag("Core:Lamp");
	TagID::IS_CHANDELIER =		TagRegistry::get_instance().add_tag("Core:Chandelier");
	TagID::IS_LIFE_CRYSTAL =	TagRegistry::get_instance().add_tag("Core:LyfeCrystal");

	//Item
	TagID::IS_COIN =			TagRegistry::get_instance().add_tag("Core:Coin");

	//Ammo
	TagID::IS_ARROW =			TagRegistry::get_instance().add_tag("Core:Arrow");
	TagID::IS_BULLET =			TagRegistry::get_instance().add_tag("Core:Bullet");

	//Weapon
	TagID::IS_PICKAXE =			TagRegistry::get_instance().add_tag("Core:Pickaxe");
	TagID::IS_SWORD =			TagRegistry::get_instance().add_tag("Core:Sword");
	TagID::IS_PIERCING =		TagRegistry::get_instance().add_tag("Core:Piercing");
	TagID::IS_AXE =				TagRegistry::get_instance().add_tag("Core:Axe");
	TagID::IS_HAMMER =			TagRegistry::get_instance().add_tag("Core:Hammer");
	TagID::IS_GUN =				TagRegistry::get_instance().add_tag("Core:Gun");
	TagID::IS_SHOTGUN =			TagRegistry::get_instance().add_tag("Core:Shotgun");
	TagID::IS_BOW =				TagRegistry::get_instance().add_tag("Core:Bow");
	TagID::IS_THROWABLE =		TagRegistry::get_instance().add_tag("Core:Throwable");
	TagID::IS_SPEAR =			TagRegistry::get_instance().add_tag("Core:Spear");
	TagID::IS_MAGICAL =			TagRegistry::get_instance().add_tag("Core:Magical");

	//Consumable
	TagID::IS_POTION =			TagRegistry::get_instance().add_tag("Core:Potion");
	TagID::IS_FOOD =			TagRegistry::get_instance().add_tag("Core:Food");

	//Enemies
	TagID::IS_SLIME =			TagRegistry::get_instance().add_tag("Core:Slime");
}