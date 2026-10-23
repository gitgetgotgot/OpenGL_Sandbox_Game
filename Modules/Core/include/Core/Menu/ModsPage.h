#pragma once
#include <vector>
#include <UI/Canvas.h>
#include <UI/SDF_Text.h>
#include <UI/ScrollView.h>
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
	static ModsPage& Instance() {
		static ModsPage page;
		return page;
	}
	void update();
	void enable(bool state);
	void setup_page();
	void load_mods_UI();
	void show_mod_info(std::string& mod_info_txt);
	void hide_mod_info();
private:
	ModsPage() = default;
	~ModsPage() = default;
	CoreUI::UI_Canvas_Ptr canvas;

	CoreUI::UI_Obj_Ptr mods_list_obj;
	CoreUI::UI_Obj_Ptr mod_description_obj;

	CoreUI::UI_Obj_Ptr mods_content;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollView> mods_view;
	CoreUI::UI_Component_Ptr<CoreUI::ScrollView> mod_description_view;
	CoreUI::UI_Component_Ptr<CoreUI::SDF_Text> mod_description;
	std::vector<ModElement> mod_elements;
	CoreAudio::AudioSource button_click_sound;
};