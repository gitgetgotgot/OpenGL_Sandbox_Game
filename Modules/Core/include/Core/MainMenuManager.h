#pragma once
#include "Core/Menu/MainPage.h"
#include "Core/Menu/MainMenuPage.h"
#include "Core/Menu/SavesPage.h"
#include "Core/Menu/SettingsPage.h"
#include "Core/Menu/ModsPage.h"
#include "Core/Menu/CreatorPage.h"

class MainMenuManager {
public:
	static MainMenuManager& Instance() {
		static MainMenuManager menu;
		return menu;
	}
	void init();
	void update();

	void start_loading_resources();
	void stop_loading_resources();

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
	MainMenuManager() = default;
	~MainMenuManager() = default;

	MainPage main_page;
	MainMenuPage main_menu_page;
	SavesPage saves_page;
	SettingsPage settings_page;
	ModsPage mods_page;
	CreatorPage creator_page;

	CoreAudio::AudioSource button_click_sound;
	CoreAudio::AudioSource music_test;
};