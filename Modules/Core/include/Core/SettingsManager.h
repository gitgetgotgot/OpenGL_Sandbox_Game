#pragma once
#include <UI/UI_Comp_Ptr.h>
#include <UI/SDF_Text.h>
#include <UI/ScrollBar.h>
#include <Audio/AudioGroupMixer.h>
#include <nlohmann/json.hpp>

class SettingsManager {
	struct Config {
		struct Screen {
			uint32_t width, height;
			bool fullscreen, vsync;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(Screen, width, height, fullscreen, vsync)
		};
		struct Volume {
			uint32_t master, music, sfx;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(Volume, master, music, sfx)
		};
		Screen screen;
		Volume volume;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, screen, volume)
	};
	friend class MainMenuManager;
public:
	static SettingsManager& Instance() {
		static SettingsManager menu;
		return menu;
	}
	void load_settings();
	void change_resolution();
	void toggle_fullscreen();
	void toggle_vsync();
	void set_master_volume(float volume);
	void set_music_volume(float volume);
	void set_sfx_volume(float volume);
	float get_master_volume();
	float get_music_volume();
	float get_sfx_volume();
	bool is_fullscreen() const { return config.screen.fullscreen; }
	bool is_vsync_on() const { return config.screen.vsync; }
	uint32_t get_res_index() const { return current_resolution_index; }
private:
	Engine::Audio::AudioGroupMixer mixer_ui;
	Engine::Audio::AudioGroupMixer mixer_music;
	Engine::Audio::AudioGroupMixer mixer_sfx;

	Config config;
	uint32_t current_resolution_index = 0;
};