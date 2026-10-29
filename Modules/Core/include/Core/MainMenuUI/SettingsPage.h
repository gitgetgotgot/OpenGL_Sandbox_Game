#pragma once
#include <Utility/TextBufferBuilder.h>
#include <UI/ScrollBar.h>
#include <UI/SDF_Text.h>
#include <UI/Canvas.h>
#include <Audio/AudioSource.h>

class SettingsPage {
public:
	static SettingsPage& Instance() {
		static SettingsPage page;
		return page;
	}
	void update();
	void enable(bool state);
	void setup_page();
private:
	SettingsPage() = default;
	~SettingsPage() = default;
	void change_resolution();
	void toggle_fullscreen();
	void toggle_vsync();
	void set_master_volume(float volume);
	void set_music_volume(float volume);
	void set_sfx_volume(float volume);
private:
	Engine::UI::UI_Canvas_Ptr canvas;

	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> resolution_text;
	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> fullscreen_text;
	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> vsync_text;

	Engine::UI::UI_Component_Ptr<Engine::UI::ScrollBar> volume_slider_master;
	Engine::UI::UI_Component_Ptr<Engine::UI::ScrollBar> volume_slider_sfx;
	Engine::UI::UI_Component_Ptr<Engine::UI::ScrollBar> volume_slider_music;

	TextBufferBuilder text_builder;
	Engine::Audio::AudioSource button_click_sound;
};