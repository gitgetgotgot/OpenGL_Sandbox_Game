#pragma once
#include <filesystem>
#include <unordered_map>
#include <UI/Canvas.h>
#include <Audio/AudioSource.h>

struct NewSaveData {
	std::string save_name;
	//other options
};

class SaveElement {
public:
	SaveElement() = default;
	SaveElement(NewSaveData& data);
	void connect_with_UI_element(CoreUI::UI_Obj_Ptr& ui_element);
	std::filesystem::path file_path;
	std::string save_name;
private:
	CoreAudio::AudioSource button_click_sound;
	CoreUI::UI_Obj_Ptr ui_element;
	void _delete_world_save();
	void _save_world();
	void _load_world_save();
};

class SavesPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
	void load_saves();
	void add_UI_save_element();
	bool create_new_save(NewSaveData& save_data);
private:
	CoreUI::UI_Canvas_Ptr canvas;
	CoreUI::UI_Obj_Ptr saves_content;
	CoreAudio::AudioSource button_click_sound;
	std::unordered_map<std::string, SaveElement> saves;
};