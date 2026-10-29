#pragma once
#include <IOSystem/InputHandler.h>
#include <Rendering/OpenGL_Renderer.h>

class GameEngine {
public:
	enum Game_State : uint8_t {
		inMainMenu, inGame, LoadingResources
	};
	~GameEngine();
	bool update();
	void render();
	void input_end_frame();
	void init();
	void main_loop();
	void uninit();
	static void switch_update_state(Game_State state);
private:
	void init_MainWindow();
	void init_OpenGL();
	void init_input();
	void init_main_resources();
	void loading_update();
	void gameplay_update();
private:
	void(GameEngine::* update_proc)() = nullptr;
	GLFWwindow* window;

	std::unique_ptr<Engine::Graphics::OpenGL_Renderer> renderer;

	inline static Game_State game_update_state = LoadingResources;
	inline static Game_State game_render_state = inMainMenu;
};