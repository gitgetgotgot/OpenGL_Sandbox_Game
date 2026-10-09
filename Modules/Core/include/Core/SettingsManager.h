#pragma once
#include <UI/UI_Comp_Ptr.h>
#include <UI/SDF_Text.h>
#include <UI/ScrollBar.h>
#include <Utility/TextBufferBuilder.h>

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
private:
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> resolution_text;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> fullscreen_text;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> vsync_text;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollBar> volume_slider_master;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollBar> volume_slider_sfx;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollBar> volume_slider_music;
	bool fullscreen = false;
	bool vsync_on = true;
	uint32_t current_resolution_index = 0;
	TextBufferBuilder text_builder;
};