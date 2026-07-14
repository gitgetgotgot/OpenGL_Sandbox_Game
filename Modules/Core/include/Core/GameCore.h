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
	/*void init_inventory_buffer();
	void init_audio();
	//inventory
	void recolor_active_bar_slot();
	void reset_active_bar_slot();
	void update_item_info_box(ObjectType type, int id);
	void remove_inventory_item(int slot_index);
	void update_main_crafting_slot(int colorVertex_id, int craft_index, int& text_info_index, Vertex2f*& buffer_ptr);
	void craft_item(int craft_id);
	std::vector<int> get_available_crafts();
	//effects
	void update_effect_info_box(Effects::Effect& effect);
	void use_item_with_effect(int inventory_slot);
	void apply_player_effect(Effects::Effect effect);
	void apply_entity_effect(Effects::Effect effect, EntityStats& stats);
	//weapons
	void activate_weapon(WeaponType type);
	bool shoot_arrow(int weapon_id);
	bool shoot_bullet(int weapon_id);
	bool shoot_magic(int weapon_id);
	bool throw_projectile(int weapon_id);
	void update_active_weapon(float deltaTime);
	//items
	void drop_item(int id, float X, float Y, int amount, float Xinc, bool pick_cd, float cd_time);
	void drop_enemy_items(int enemy_id, float xPos, float yPos);
	bool try_to_pick_item(DroppedItem& item);
	bool item_is_stackable(int id, ObjectType type);
	//world objects
	void update_lighting(glm::vec2 light_pos, glm::vec3 light_color, float radius, bool add);
	void update_block_light_impact(int column, int line, bool use_recursive_method, int recursive_depth);
	bool is_solid_block(int column, int line);
	void set_block(int x, int y, int id);
	void destroy_object(int x, int y);
	void destroy_complex_object(int x, int y);
	void set_wall(int x, int y, unsigned short id);
	void destroy_wall(int x, int y);
	//world
	void create_the_world_thread();
	void load_the_world_thread();
	void exit_and_save_the_world_thread();
	void load_available_saves();*/
	
private:
	GLFWwindow* window;

	const int MAX_CRAFTS_AVAILABLE = 12;
	bool collisionIsOn = true;
	//camera(Player) values
	float playerXinc = 0.f, playerYinc = 0.f;
	Player player;
	//inventory
	
	ActiveWeapon active_weapon;
	ActiveBreakableObject active_breakable_object;
	EntityInfoText entity_info_text;
	short active_bar_slot = 0; //from 0 to 9

	//Ambient objects
	//AmbientController ambientController;
	//Effects
	Effects::EffectsManager* effectsManager = nullptr;
	//Components manager
	ComponentsManager* compsMgr = nullptr;
	//Particles
	ParticlesManager* particlesMgr = nullptr;
	ParticleSystem* particlesSystem = nullptr;
	//Entities
	EntitySystem* entitySystem = nullptr;
	//Recipes
	CraftableItem craftable_items[50];
	CraftingSystem crafting_system;
	//Game Time
	TimeManager timeMgr;
	//Main camera
	Camera camera;

	//MAIN ARRAYS AND VECTORS
	std::vector<DroppedItem> dropped_items;
	std::vector<DamageText> damage_text;

	//textures
	Texture atlas_texture;
	SpriteManager* spriteMgr = nullptr;

	//Graphics main objects
	std::unique_ptr<OpenGL_Renderer> renderer;
	std::unique_ptr<UBO> universal_ubo;

	//World data
	std::unique_ptr<World> world;
	//ui
	UI_Renderer* ui_renderer = nullptr;

	//
	//Smart_ptr<ShaderProgram> ambient_sprite_SP_ptr;
	//Smart_ptr<ShaderProgram> UI_sprite_SP_ptr;
	//
	//Smart_ptr<SSBO> sprite_ambient_ssbo_p;
	//Smart_ptr<SSBO> spriteLightMapSSBO;

	//

	SpriteData ambient_sprite_buf[20]; //20 ambient sprites

	int entities_count = 0;
	int index_size = 0;
	int buffer_size = 0;

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