#pragma once
#include <UI/UI_Comp_Ptr.h>
#include <UI/SDF_Text.h>
#include <UI/ScrollBar.h>
#include <Audio/AudioGroupMixer.h>

class SettingsManager {
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
	bool is_fullscreen() const { return fullscreen; }
	bool is_vsync_on() const { return vsync_on; }
	uint32_t get_res_index() const { return current_resolution_index; }
private:
	CoreAudio::AudioGroupMixer mixer_ui;
	CoreAudio::AudioGroupMixer mixer_music;
	CoreAudio::AudioGroupMixer mixer_sfx;

	bool fullscreen = false;
	bool vsync_on = true;
	uint32_t current_resolution_index = 0;
};