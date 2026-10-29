#include "Core/MainMenuUI/MainMenuPage.h"
#include "Core/MainMenuUI/MenuUIController.h"
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
	canvas = Engine::UI::UI_Creator::create_canvas();

	uint32_t button64_sprite_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t white_sprite_id = Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:White").value();

	auto button_play = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.4f), true, button64_sprite_id,
		true, "SinglePlay", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_settings = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.275f), true, button64_sprite_id,
		true, "Settings", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_mods = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.15f), true, button64_sprite_id,
		true, "Mods", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_exit = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.025f), true, button64_sprite_id,
		true, "Exit", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);

	//auto logo_frame = CoreUI::UI_Creator::create_image(white_sprite_id, glm::vec2(1.5f, 0.3f), glm::vec2(0.0f, 0.8f), glm::vec4(1.0f));
	auto menu_logo = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(1.5f, 0.3f), glm::vec2(0.0f, 0.8f), "Sandbox 2D", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f
	);
	auto _menu_logo_text = menu_logo->get_component<Engine::UI::SDF_Text>();
	_menu_logo_text->set_alignment(Engine::UI::SDF_Text::TextHorizAlign::Center_Align, Engine::UI::SDF_Text::TextVertAlign::Middle_Align);

	auto version = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(-SystemContext::display.ratio + 0.525f, -0.95f), "Version 1.0.0", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.075f
	);

	button_play->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<MenuUIController, &MenuUIController::open_saves_page>(&MenuUIController::Instance());
	button_settings->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<MenuUIController, &MenuUIController::open_settings_page>(&MenuUIController::Instance());
	button_mods->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<MenuUIController, &MenuUIController::open_mods_page>(&MenuUIController::Instance());
	button_exit->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<MenuUIController, &MenuUIController::exit_game>(&MenuUIController::Instance());

	canvas->add_object(button_play);
	canvas->add_object(button_settings);
	canvas->add_object(button_mods);
	canvas->add_object(button_exit);
	//canvas->add_object(logo_frame);
	canvas->add_object(menu_logo);
	canvas->add_object(version);
}