#include "Core/Menu/MainMenuPage.h"
#include "Core/MainMenuManager.h"
#include <UI/Button.h>
#include <UI/SDF_Text.h>
#include <IOSystem/SystemContext.h>
#include <Resources/Sprite.h>
#include <UI/UI_Creator.h>

void MainMenuPage::update() {

}

void MainMenuPage::enable(bool state) {
	canvas->is_enabled = state;
}

void MainMenuPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	uint32_t button_sprite_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:Button64").value();
	uint32_t sprite_white_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:White").value();

	auto button_play = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.4f), true, button_sprite_id,
		true, "Play", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_settings = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.275f), true, button_sprite_id,
		true, "Settings", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_mods = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.15f), true, button_sprite_id,
		true, "Mods", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_exit = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.025f), true, button_sprite_id,
		true, "Exit", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);

	auto menu_logo = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.8f), "Main Menu", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f
	);
	auto _menu_logo_text = menu_logo->get_component<CoreUI::SDF_Text>();
	_menu_logo_text->set_alignment(CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);

	auto version = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(0.05f, 0.1f), glm::vec2(-SystemContext::display.ratio + 0.05f, -0.95f), "Version 1.0.0", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.075f
	);

	button_play->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_saves_page>(&MainMenuManager::Instance());
	button_settings->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_settings_page>(&MainMenuManager::Instance());
	button_mods->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_mods_page>(&MainMenuManager::Instance());
	button_exit->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::exit_game>(&MainMenuManager::Instance());

	canvas->add_object(button_play);
	canvas->add_object(button_settings);
	canvas->add_object(button_mods);
	canvas->add_object(button_exit);
	canvas->add_object(menu_logo);
	canvas->add_object(version);
}