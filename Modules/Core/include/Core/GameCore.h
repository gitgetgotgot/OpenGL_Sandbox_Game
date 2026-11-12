#pragma once
#include <cstdint>

class GameEngine {
public:
	enum Game_State : uint8_t {
		inMainMenu, inGame, LoadingResources
	};
	~GameEngine();
	bool update();
	void render();
	bool init();
	void main_loop();
	void uninit();
	static void switch_update_state(Game_State state);
private:
	bool init_MainWindow();
	bool init_GraphicsAPI();
	bool init_input();
	bool init_main_resources();
	void loading_update();
	void gameplay_update();
private:
	void(GameEngine::* update_proc)() = nullptr;

	inline static Game_State game_update_state = LoadingResources;
	inline static Game_State game_render_state = inMainMenu;
};