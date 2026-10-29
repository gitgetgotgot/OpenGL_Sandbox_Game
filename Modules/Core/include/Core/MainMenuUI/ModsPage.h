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
	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> toggle_button_text;
	inline static Engine::Audio::AudioSource button_click_sound;
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
	Engine::UI::UI_Canvas_Ptr canvas;

	Engine::UI::UI_Obj_Ptr mods_list_obj;
	Engine::UI::UI_Obj_Ptr mod_description_obj;

	Engine::UI::UI_Obj_Ptr mods_content;
	Engine::UI::UI_Component_Ptr<Engine::UI::ScrollView> mods_view;
	Engine::UI::UI_Component_Ptr<Engine::UI::ScrollView> mod_description_view;
	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> mod_description;
	std::vector<ModElement> mod_elements;
	Engine::Audio::AudioSource button_click_sound;
};