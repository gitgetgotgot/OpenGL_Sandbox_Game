#pragma once
#include <Utility/TextBufferBuilder.h>
#include <UI/ScrollBar.h>
#include <UI/SDF_Text.h>
#include <UI/Canvas.h>
#include <Audio/AudioSource.h>

class SettingsPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
private:
	void change_resolution();
	void toggle_fullscreen();
	void toggle_vsync();
	void set_master_volume(float volume);
	void set_music_volume(float volume);
	void set_sfx_volume(float volume);

	CoreUI::UI_Canvas_Ptr canvas;

	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> resolution_text;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> fullscreen_text;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> vsync_text;

	CoreUI::UI_Component_Ptr<CoreUI::ScrollBar> volume_slider_master;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollBar> volume_slider_sfx;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollBar> volume_slider_music;

	TextBufferBuilder text_builder;
	CoreAudio::AudioSource button_click_sound;
};