#include "Core/Menu/SavesPage.h"
#include "Core/MainMenuManager.h"
#include "Core/WorldManager.h"
#include <UI/ScrollView.h>
#include <UI/Button.h>
#include <Resources/Sprite.h>
#include <UI/UI_Creator.h>
#include <UI/UI_ObjectManager.h>
#include <fstream>

SaveElement::SaveElement(SaveData& data) {
	save_name = data.save_name;
	file_path = data.file_path;
}

void SaveElement::connect_with_UI_element(CoreUI::UI_Obj_Ptr& ui_element) {
	this->ui_element = ui_element;
}

void SaveElement::_delete_world_save() {
	button_click_sound.play();
}

void SaveElement::_save_world() {

}

void SaveElement::_load_world_save() {
	button_click_sound.play();
	MainMenuManager::Instance().start_world_loading();
	WorldManager::Instance().load_world(file_path);
}

void SavesPage::update() {
	
}

void SavesPage::enable(bool state) {
	canvas->is_enabled = state;
}

void SavesPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	button_click_sound.setup_source("Core:Button");
	SaveElement::button_click_sound = button_click_sound;

	uint32_t gradient2_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:Gradient2").value();
	uint32_t button64_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t frame0_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame0").value();

	auto button_back = CoreUI::UI_Creator::create_button_image(
		glm::vec2(1.0f, 0.1f), glm::vec2(-0.56f, -0.85f), true, button64_id,
		true, "Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_create = CoreUI::UI_Creator::create_button_image(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.56f, -0.85f), true, button64_id,
		true, "Create", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);

	button_back->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_saves_page>(&MainMenuManager::Instance());
	button_create->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_creator_page>(&MainMenuManager::Instance());

	//saves background
	auto label_background_obj = CoreUI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(1.0f, 0.16f), glm::vec2(0.0f, 0.8f), glm::vec4(1.0f)
	);

	auto saves_label = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f), "Saved Worlds", glm::vec4(1.0f), 0.075f
	);
	saves_label->get_component<CoreUI::SDF_Text>()->set_alignment(CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);
	label_background_obj.add_child(saves_label);

	auto saves_background_obj = CoreUI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(2.12f, 1.52f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	saves_background_obj->get_component<CoreUI::Image>()->set_border_size(0.06f);

	// scroll_view
	auto saves_scroll_view = CoreUI::UI_Creator::create_scroll_view(
		glm::vec2(2.0f, 1.4f), glm::vec2(0.0f)
	);
	auto scroll_view = saves_scroll_view->get_component<CoreUI::ScrollView>();
	scroll_view->set_group_layout(CoreUI::ScrollView::GroupLayout::LAYOUT_VERTICAL);
	saves_view = scroll_view;
	// get content for scroll_view
	saves_content = saves_scroll_view.get_child_by_index(0);
	// vertical scroll_bar for scroll_view
	auto scroll_bar_Y = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 0.08f), glm::vec2(0.96f, 0.0f),
		false, true, gradient2_id, button64_id, glm::vec4(0.471f, 0.376f, 0.314f, 1.0f), glm::vec4(1.0f), true, false
	);
	auto scrollbar = scroll_bar_Y->get_component<CoreUI::ScrollBar>();
	scroll_view->has_vertical_scrollbar = true;
	scroll_view->bind_vertical_scrollbar(scrollbar);

	saves_scroll_view.add_child(scroll_bar_Y);

	canvas->add_object(label_background_obj);
	canvas->add_object(saves_background_obj);
	canvas->add_object(button_back);
	canvas->add_object(button_create);
	canvas->add_object(saves_scroll_view);
}

void SavesPage::load_saves() {
	std::filesystem::path root = RESOURCE_ROOT;
	std::filesystem::path saves_path = root / "Saves";
	if (!std::filesystem::exists(saves_path)) {
		std::filesystem::create_directory(saves_path);
		return;
	}
	for (auto& entry : std::filesystem::directory_iterator(saves_path)) {
		std::filesystem::path entry_path = entry.path();
		if (std::filesystem::is_directory(entry_path)) {
			// load save and read data
			SaveData data;
			data.file_path = entry_path;
			data.save_name = entry_path.filename().string();
			saves.emplace(data.save_name, SaveElement(data));
			add_UI_save_element(saves[data.save_name]);
		}
	}
	saves_view->force_update_content();
}

void SavesPage::load_save(const std::string& save_name) {
	std::filesystem::path root = RESOURCE_ROOT;
	std::filesystem::path save_path = root / "Saves" / save_name;
	if (!std::filesystem::exists(save_path)) {
		return;
	}
	SaveData data;
	data.file_path = save_path;
	data.save_name = save_name;
	saves.emplace(data.save_name, SaveElement(data));
	add_UI_save_element(saves[data.save_name]);

	saves_view->force_update_content();
}

CoreUI::UI_Obj_Ptr SavesPage::add_UI_save_element(SaveElement& save) {
	// prepare sprites IDs
	uint32_t button32_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button32p").value();
	uint32_t sprite_play_button_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:UI_ButtonPlay").value();
	uint32_t sprite_delete_button_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:UI_ButtonDelete").value();

	// create UI save element
	auto save_obj = CoreUI::UI_Creator::create_image9sliced(button32_id, glm::vec2(1.9f, 0.35f), glm::vec2(0.0f), glm::vec4(0.441, 0.379, 0.332, 1.0f));
	auto button_play = CoreUI::UI_Creator::create_button_image(glm::vec2(0.1f, 0.1f), glm::vec2(-0.85f, -0.1f), false, sprite_play_button_id);
	auto button_delete = CoreUI::UI_Creator::create_button_image(glm::vec2(0.1f, 0.1f), glm::vec2(-0.73f, -0.1f), false, sprite_delete_button_id);
	auto save_name = CoreUI::UI_Creator::create_SDF_text(glm::vec2(1.5f, 0.1f), glm::vec2(-0.15f, 0.1f), save.save_name, glm::vec4(1.0f), 0.07f);
	save_obj.add_child(button_play);
	save_obj.add_child(button_delete);
	save_obj.add_child(save_name);

	// add element to scroll_view content
	saves_content.add_child(save_obj);

	// connect SaveElement with UI save element (SaveElement should have access to UI object)
	save.connect_with_UI_element(save_obj);

	// connect buttons with SaveElement methods
	button_play->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SaveElement, &SaveElement::_load_world_save>(&save);
	button_delete->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SaveElement, &SaveElement::_delete_world_save>(&save);

	// return created UI save element
	return save_obj;
}

bool SavesPage::world_save_exists(std::string& save_name) {
	if (auto it = saves.find(save_name); it != saves.end()) {
		return true;
	}
	else return false;
}