#include "Resources/CoreTags.h"
#include "Resources/TagRegistry.h"

void Engine::Resource::Tag::register_core_tags() {
	//Block
	IS_SOLID_BLOCK =	TagRegistry::Instance().add_tag("Core:SolidBlock");
	IS_WOOD =			TagRegistry::Instance().add_tag("Core:Wood");
	IS_TREE_TOP =		TagRegistry::Instance().add_tag("Core:TreeTop");
	IS_PLATFORM =		TagRegistry::Instance().add_tag("Core:Platform");
	IS_TORCH =			TagRegistry::Instance().add_tag("Core:Torch");
	IS_LIQUID =			TagRegistry::Instance().add_tag("Core:Liquid"); //should probably make new Object for liquids later
	IS_GRASS =			TagRegistry::Instance().add_tag("Core:Grass");
	IS_PLANT =			TagRegistry::Instance().add_tag("Core:Plant");
	IS_GLASS =			TagRegistry::Instance().add_tag("Core:Glass");
	IS_BOTTLE =			TagRegistry::Instance().add_tag("Core:Bottle");

	//Multiblock
	IS_WORKBENCH =		TagRegistry::Instance().add_tag("Core:Workbench");
	IS_FURNACE =		TagRegistry::Instance().add_tag("Core:Furnace");
	IS_CHEST =			TagRegistry::Instance().add_tag("Core:Chest");
	IS_DOOR =			TagRegistry::Instance().add_tag("Core:Door");
	IS_TABLE =			TagRegistry::Instance().add_tag("Core:Table");
	IS_CHAIR =			TagRegistry::Instance().add_tag("Core:Chair");
	IS_ANVIL =			TagRegistry::Instance().add_tag("Core:Anvil");
	IS_LAMP =			TagRegistry::Instance().add_tag("Core:Lamp");
	IS_CHANDELIER =		TagRegistry::Instance().add_tag("Core:Chandelier");
	IS_LIFE_CRYSTAL =	TagRegistry::Instance().add_tag("Core:LyfeCrystal");

	//Item
	IS_COIN =			TagRegistry::Instance().add_tag("Core:Coin");

	//Ammo
	IS_ARROW =			TagRegistry::Instance().add_tag("Core:Arrow");
	IS_BULLET =			TagRegistry::Instance().add_tag("Core:Bullet");

	//Weapon
	IS_PICKAXE =		TagRegistry::Instance().add_tag("Core:Pickaxe");
	IS_SWORD =			TagRegistry::Instance().add_tag("Core:Sword");
	IS_PIERCING =		TagRegistry::Instance().add_tag("Core:Piercing");
	IS_AXE =			TagRegistry::Instance().add_tag("Core:Axe");
	IS_HAMMER =			TagRegistry::Instance().add_tag("Core:Hammer");
	IS_GUN =			TagRegistry::Instance().add_tag("Core:Gun");
	IS_SHOTGUN =		TagRegistry::Instance().add_tag("Core:Shotgun");
	IS_BOW =			TagRegistry::Instance().add_tag("Core:Bow");
	IS_THROWABLE =		TagRegistry::Instance().add_tag("Core:Throwable");
	IS_SPEAR =			TagRegistry::Instance().add_tag("Core:Spear");
	IS_MAGICAL =		TagRegistry::Instance().add_tag("Core:Magical");

	//Consumable
	IS_POTION =			TagRegistry::Instance().add_tag("Core:Potion");
	IS_FOOD =			TagRegistry::Instance().add_tag("Core:Food");

	//Enemies
	IS_SLIME =			TagRegistry::Instance().add_tag("Core:Slime");
}