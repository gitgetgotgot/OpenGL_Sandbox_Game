#pragma once
#include <filesystem>
#include <unordered_map>
#include <UI/Canvas.h>
#include <Audio/AudioSource.h>

struct SaveData {
	std::string save_name;
	std::filesystem::path file_path;
	//other options
};

class SaveElement {
public:
	SaveElement() = default;
	SaveElement(SaveData& data);
	void connect_with_UI_element(CoreUI::UI_Obj_Ptr& ui_element);
	void _delete_world_save();
	void _save_world();
	void _load_world_save();
	std::filesystem::path file_path;
	std::string save_name;
	inline static CoreAudio::AudioSource button_click_sound;
private:
	CoreUI::UI_Obj_Ptr ui_element;
};

class SavesPage {
public:
	void update();
	void enable(bool state);
	void setup_page();
	void load_saves();
	CoreUI::UI_Obj_Ptr add_UI_save_element(SaveElement& save);
	bool create_new_save(SaveData& save_data);
private:
	CoreUI::UI_Canvas_Ptr canvas;
	CoreUI::UI_Obj_Ptr saves_content;
	CoreAudio::AudioSource button_click_sound;
	std::unordered_map<std::string, SaveElement> saves;
};