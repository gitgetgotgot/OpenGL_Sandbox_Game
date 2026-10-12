#pragma once
#include "World.h"
#include <IOSystem/InputHandler.h>

class GameEngine {
	enum Game_State : uint8_t {
		inMainMenu, inWorld, LoadingResources
	};
public:
	~GameEngine();
	bool update();
	void render();
	void input_end_frame();
	void init();
	void main_loop();
	void uninit();
	
	void init_MainWindow();
	void init_OpenGL();
	void init_input();
	void init_main_resources();
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