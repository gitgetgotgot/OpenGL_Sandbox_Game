#pragma once
#include <Audio/AudioSource.h>

class GameUIController {
private:
	enum GameMenuState : uint8_t {
		in_GamePlay, in_GamePause, in_GameSettings
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

	void exit_to_main_menu();
private:
	GameUIController() = default;
	~GameUIController() = default;

	Engine::Audio::AudioSource button_click_sound;
	GameMenuState current_state = GameMenuState::in_GamePlay;
};