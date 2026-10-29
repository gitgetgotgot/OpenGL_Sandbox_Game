#pragma once
#include <Audio/AudioSource.h>

class MenuUIController {
private:
	enum MainMenuState : uint8_t {
		in_MainMenu, in_Saves, in_Creator, in_Settings, in_Mods,
		Loading_World, Generating_World, Saving_World
	};
public:
	static MenuUIController& Instance() {
		static MenuUIController menu;
		return menu;
	}
	void init();
	void update();

	void reload_resources();
	void start_loading_resources();
	void stop_loading_resources();

	void start_world_generation();
	void stop_world_generation();

	void start_world_loading();
	void stop_world_loading();

	void start_world_saving();
	void stop_world_saving();

	void load_saves();
	void load_mods();

	void open_main_menu_page();
	void close_main_menu_page();

	void exit_game();

	void open_settings_page();
	void close_settings_page();

	void open_saves_page();
	void close_saves_page();

	void open_mods_page();
	void close_mods_page();

	void open_creator_page();
	void close_creator_page();
private:
	MenuUIController() = default;
	~MenuUIController() = default;

	Engine::Audio::AudioSource button_click_sound;
	Engine::Audio::AudioSource music_test;
	MainMenuState current_state = MainMenuState::in_MainMenu;
};