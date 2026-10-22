#pragma once
#include <vector>
#include <UI/Canvas.h>
#include <UI/SDF_Text.h>
#include <Audio/AudioSource.h>

class ModElement {
public:
	ModElement() = default;
	void _toggle_mod();
	void _show_mod_info();

	uint32_t mod_id = 0;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> toggle_button_text;
	inline static CoreAudio::AudioSource button_click_sound;
};

class ModsPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
	void load_mods_UI();
private:
	CoreUI::UI_Canvas_Ptr canvas;
	CoreUI::UI_Obj_Ptr mods_content;
	std::vector<ModElement> mod_elements;
};