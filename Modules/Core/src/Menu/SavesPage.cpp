#include "Core/Menu/SavesPage.h"
#include "Core/MainMenuManager.h"
#include <UI/ScrollView.h>
#include <UI/Button.h>
#include <Resources/Sprite.h>
#include <UI/UI_Creator.h>
#include <UI/UI_ObjectManager.h>

#include <fstream>

void SavesPage::update() {

}

void SavesPage::enable(bool state) {
	canvas->is_enabled = state;
}

void SavesPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	button_click_sound.setup_source("Core:Button");

	uint32_t tooltip_sprite_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:Tooltip32").value();
	uint32_t gradient2_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:Gradient2").value();
	uint32_t button32_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:Button32").value();
	uint32_t button64_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:Button64").value();
	uint32_t background_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:Background1").value();
	uint32_t sprite_white_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:White").value();

	auto button_back = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(1.0f, 0.1f), glm::vec2(-0.56f, -0.85f), button64_id,
		"Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_create = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.56f, -0.85f), button64_id,
		"Create", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);

	button_back->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_saves_page>(&MainMenuManager::Instance());
	button_create->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_creator_page>(&MainMenuManager::Instance());

	//saves background
	auto saves_background_obj = CoreUI::UI_Creator::create_image9sliced(
		background_id, glm::vec2(2.12f, 1.52f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	saves_background_obj->get_component<CoreUI::Image>()->set_border_size(0.06f);

	// panel for clipping
	auto saves_panel = CoreUI::UI_Creator::create_panel(
		glm::vec2(2.0f, 1.4f), glm::vec2(0.0f), sprite_white_id, glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)
	);
	// scroll_view
	auto saves_scroll_view = CoreUI::UI_Creator::create_scroll_view(
		glm::vec2(2.0f, 1.4f), glm::vec2(0.0f)
	);
	auto scroll_view = saves_scroll_view->get_component<CoreUI::ScrollView>();
	scroll_view->set_group_layout(CoreUI::ScrollView::GroupLayout::LAYOUT_VERTICAL);
	// get content for scroll_view
	saves_content = saves_scroll_view.get_child_by_index(0);
	// vertical scroll_bar for scroll_view
	auto scroll_bar_Y = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 0.08f), glm::vec2(0.96f, 0.0f),
		false, true, gradient2_id, button64_id, glm::vec4(0.471f, 0.376f, 0.314f, 1.0f), glm::vec4(0.639f, 0.541f, 0.475f, 1.0f), true
	);
	auto scrollbar = scroll_bar_Y->get_component<CoreUI::ScrollBar>();
	scroll_view->has_vertical_scrollbar = true;
	scroll_view->bind_vertical_scrollbar(scrollbar);

	saves_scroll_view.add_child(scroll_bar_Y);

	saves_panel.add_child(saves_scroll_view);

	canvas->add_object(saves_background_obj);
	canvas->add_object(button_back);
	canvas->add_object(button_create);
	canvas->add_object(saves_panel);

	for (int i = 0; i < 10; ++i) {
		auto save_image_obj = CoreUI::UI_Creator::create_image9sliced(
			button32_id, glm::vec2(1.9f, 0.35f), glm::vec2(0.0f), glm::vec4(0.441, 0.379, 0.332, 1.0f)
		);
		saves_content.add_child(save_image_obj);
	}
}

void SavesPage::load_saves() {
	std::filesystem::path saves_path = RESOURCE_ROOT;
	saves_path /= "Saves";
	if (!std::filesystem::exists(saves_path)) {
		std::filesystem::create_directory(saves_path);
		return;
	}
	for (auto& entry : std::filesystem::directory_iterator(saves_path)) {
		std::filesystem::path entry_path = entry.path();
		if (std::filesystem::is_regular_file(entry_path)) {
			// load save and read data
			NewSaveData data;

		}
	}
}

void SavesPage::add_UI_save_element() {

}

bool SavesPage::create_new_save(NewSaveData& save_data) {
	if (auto it = saves.find(save_data.save_name); it != saves.end()) {
		return false; // save with such name already exists
	}

	SaveElement new_save_element;

	//saves.emplace(save_data.save_name, )
}