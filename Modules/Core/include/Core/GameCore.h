#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <Audio/AudioManager.h>
#include "World.h"
#include <Utility/TimeManager.h>
#include <IOSystem/InputHandler.h>
#include <filesystem>

enum Game_State : uint8_t { inMainMenu, inOptions, inControlsOptions, inAudioOptions, inRenderingOptions,
	inWorldExplorer, WorldIsLoading, WorldIsSaving, WorldIsCreating, inWorldCreator, inGame, inGamePause };

class Game {
public:
	~Game();
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
	/*
	//items
	void drop_item(int id, float X, float Y, int amount, float Xinc, bool pick_cd, float cd_time);
	void drop_enemy_items(int enemy_id, float xPos, float yPos);
	bool try_to_pick_item(DroppedItem& item);
	//world
	void create_the_world_thread();
	void load_the_world_thread();
	void exit_and_save_the_world_thread();
	void load_available_saves();
	*/
	
private:
	GLFWwindow* window;

	Player player;

	//Game Time
	TimeManager timeMgr;
	//Main camera
	Camera camera;

	//Graphics main objects
	std::unique_ptr<OpenGL_Renderer> renderer;
	std::unique_ptr<UBO> universal_ubo;

	//World data
	std::unique_ptr<World> world;

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