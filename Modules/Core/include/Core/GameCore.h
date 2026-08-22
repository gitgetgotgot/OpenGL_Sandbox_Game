#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <Audio/AudioManager.h>
#include "World.h"
#include <Utility/TimeManager.h>
#include <IOSystem/InputHandler.h>
#include <UI/UI_Renderer.h>

enum Game_State : uint8_t { inMainMenu, inOptions, inControlsOptions, inAudioOptions, inRenderingOptions,
	inWorldExplorer, WorldIsLoading, WorldIsSaving, WorldIsCreating, inWorldCreator, inGame, inGamePause };

class Game {
public:
	bool update();
	void render();
	void input_end_frame();
	void toggle_Fullscreen();

	void init();
	void main_loop();
	void uninit();
	//init
	void init_open_gl();
	void init_input();
	bool load_sprites_JSON(std::unordered_map<std::string, uint32_t>& texture_layers);
	bool load_lights_JSON();
	bool load_particles_JSON();
	bool load_effects_JSON();
	bool load_crafts_JSON();
	bool load_items_basic_data_JSON();
	void load_entities_basic_data_JSON();
	void load_animations_basic_data_JSON();

	bool resolve_items_dependencies_JSON();
	bool resolve_entities_dependencies_JSON();

	/*void init_inventory_buffer();
	//items
	void drop_item(int id, float X, float Y, int amount, float Xinc, bool pick_cd, float cd_time);
	void drop_enemy_items(int enemy_id, float xPos, float yPos);
	bool try_to_pick_item(DroppedItem& item);
	bool item_is_stackable(int id, ObjectType type);
	//world
	void create_the_world_thread();
	void load_the_world_thread();
	void exit_and_save_the_world_thread();
	void load_available_saves();*/
	
private:
	GLFWwindow* window;

	Player player;
	
	ActiveWeapon active_weapon;
	ActiveBreakableObject active_breakable_object;
	EntityInfoText entity_info_text;

	//Effects
	Effects::EffectsManager* effectsManager = nullptr;
	//Components manager
	ComponentsManager* compsMgr = nullptr;
	//Crafting
	CraftingSystem* craft_sys = nullptr;
	//Particles
	ParticlesManager* particlesMgr = nullptr;
	ParticleSystem* particlesSystem = nullptr;
	//Entities
	GameEntity::EntitySystem* entitySystem = nullptr;
	//Game Time
	TimeManager timeMgr;
	//Main camera
	Camera camera;

	//MAIN ARRAYS AND VECTORS
	std::vector<GameEntity::DroppedItem> dropped_items;
	std::vector<DamageText> damage_text;

	//textures
	std::unique_ptr<Texture3D> textures_array;
	SpriteManager* spriteMgr = nullptr;

	//Graphics main objects
	std::unique_ptr<OpenGL_Renderer> renderer;
	std::unique_ptr<UBO> universal_ubo;

	//World data
	std::unique_ptr<World> world;
	//ui
	UI_Renderer* ui_renderer = nullptr;

	SpriteData ambient_sprite_buf[20]; //20 ambient sprites

	//save the world info in file in specific interval, for the safety :)
	int auto_save_interval = 120; //120 seconds

	//save files vector
	std::vector<std::string> save_Files;
	std::string active_world = "";
	bool loading_the_world = false;
	bool saving_the_world = false;
	bool creating_the_world = false;
	//game state
	Game_State game_update_state = inMainMenu, game_render_state = inMainMenu;
};