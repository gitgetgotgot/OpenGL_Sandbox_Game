#pragma once
#include <Audio/AudioSource.h>
#include <UI/Canvas.h>

class GameUIController {
private:
	enum GameMenuState : uint8_t {
		in_GamePlay, in_GamePlayConsole, in_GamePause, in_GameSettings
	};
public:
	static GameUIController& Instance() {
		static GameUIController c;
		return c;
	}
	void init();
	void update();
	void uninit();

	void open_pause_page();
	void close_pause_page();

	void open_settings_page();
	void close_settings_page();

	void show_HUD();
	void hide_HUD();
	void show_inventory();
	void hide_inventory();

	void notify_exit_to_main_menu();
private:
	void exit_to_main_menu();
	void gameplay_update();
private:
	GameUIController() = default;
	~GameUIController() = default;

	Engine::UI::UI_Canvas_Ptr canvas_background;
	Engine::Audio::AudioSource button_click_sound;
	GameMenuState current_state = GameMenuState::in_GamePlay;
	bool should_exit_to_main_menu = false;
};