#pragma once
#include "World.h"
#include <IOSystem/InputHandler.h>

enum Game_State : uint8_t { inMainMenu, inOptions, inControlsOptions, inAudioOptions, inRenderingOptions,
	inWorldExplorer, WorldIsLoading, WorldIsSaving, WorldIsCreating, inWorldCreator, inWorld, inGamePause };

class Game {
public:
	~Game();
	bool update();
	void render();
	void input_end_frame();
	void init();
	void main_loop();
	void uninit();
	//init
	void init_open_gl();
	void init_input();
	
private:
	GLFWwindow* window;

	Player player;

	//Main camera
	Camera camera;

	//Graphics main objects
	std::unique_ptr<OpenGL_Renderer> renderer;
	std::unique_ptr<UBO> universal_ubo;

	//World data
	std::unique_ptr<World> world;

	//game state
	Game_State game_update_state = inMainMenu, game_render_state = inMainMenu;
};