#pragma once
#include <UI/Canvas.h>
#include <Audio/AudioSource.h>

class ModsPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
	void load_mods_UI();
private:
	CoreUI::UI_Canvas_Ptr canvas;
	CoreUI::UI_Obj_Ptr mods_content;
	CoreAudio::AudioSource button_click_sound;
};