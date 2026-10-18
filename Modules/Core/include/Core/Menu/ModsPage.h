#pragma once
#include <UI/Canvas.h>
#include <Audio/AudioSource.h>

class ModsPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
private:
	CoreUI::UI_Canvas_Ptr canvas;
	CoreAudio::AudioSource button_click_sound;
};