#pragma once
#include <UI/Canvas.h>
#include <UI/SDF_Text.h>
#include <Audio/AudioSource.h>

class MainMenuManager {
public:
	static MainMenuManager& Instance() {
		static MainMenuManager menu;
		return menu;
	}
	void init();
	void update();

	void start_loading();
	void stop_loading();
	void open_main_page();
	void close_main_page();
private:
	MainMenuManager() = default;
	~MainMenuManager() = default;

	void setup_main_canvas();
	void setup_main_page();
	void setup_settings_page();
	void setup_saves_page();
	void setup_mods_page();
	void setup_creator_page();

	void exit_game();
	void open_settings_page();
	void close_settings_page();
	void open_saves_page();
	void close_saves_page();
	void open_mods_page();
	void close_mods_page();
	void open_creator_page();
	void close_creator_page();

	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> loading_text;
	bool is_loading = false;
	float time_loading = 0.0f;
	uint32_t phase = 0;

	CoreUI::UI_Canvas_Ptr canvas_main;
	CoreUI::UI_Canvas_Ptr canvas_main_menu;
	CoreUI::UI_Canvas_Ptr canvas_settings;
	CoreUI::UI_Canvas_Ptr canvas_saves;
	CoreUI::UI_Canvas_Ptr canvas_mods;
	CoreUI::UI_Canvas_Ptr canvas_creator;

	CoreAudio::AudioSource button_click_sound;
	CoreAudio::AudioSource music_test;
};