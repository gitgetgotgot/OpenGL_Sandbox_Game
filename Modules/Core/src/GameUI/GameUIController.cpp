#include "Core/GameUI/GameUIController.h"
#include "Core/GameUI/Pause_UI.h"
#include "Core/GameUI/HUD_UI.h"
#include "Core/GameUI/Inventory_UI.h"
#include "Core/MainMenuUI/MenuUIController.h"
#include "Core/MainMenuUI/SettingsPage.h"

#include "Core/World/WorldManager.h"
#include <UI/UI_Creator.h>
#include <Resources/Sprite.h>
#include <DeviceSystem/DeviceContext.h>

void GameUIController::init() {
	button_click_sound.setup_source("Core:Button");
	Pause_UI::Instance().setup();
	Pause_UI::Instance().enable(false);
	HUD_UI::Instance().setup();
	HUD_UI::Instance().enable(true);

	canvas_background = Engine::UI::UI_Creator::create_canvas();
	canvas_background->set_layer_index(1); //should be rendered above HUD or Additional info canvas
	canvas_background->is_enabled = false;

	uint32_t white_id = Engine::Resource::SpriteManager::Instance().get_sprite_id("Core:White").value();
	auto menu_background = Engine::UI::UI_Creator::create_image(
		white_id, glm::vec2(4.0f, 2.0f), glm::vec2(0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)
	);
	canvas_background->add_object(menu_background);
}

void GameUIController::update() {
	switch (current_state) {
	case GameMenuState::in_GamePlay:
		gameplay_update();
		break;
	case GameMenuState::in_GameSettings:
		if (DeviceContext::keyBoard.key_is_pressed(Key::KeyEscape))
			close_settings_page();
		break;
	case GameMenuState::in_GamePlayConsole:
		if (DeviceContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
			HUD_UI::Instance().enable_console(false);
			current_state = GameMenuState::in_GamePlay;
		}
		break;
	case GameMenuState::in_GamePause:
		if (should_exit_to_main_menu) {
			exit_to_main_menu();
			return;
		}
		Pause_UI::Instance().update();
		break;
	}
}

void GameUIController::uninit() {

}

void GameUIController::open_pause_page() {
	Pause_UI::Instance().enable(true);
	canvas_background->is_enabled = true;
	current_state = GameMenuState::in_GamePause;
	button_click_sound.play();
}

void GameUIController::close_pause_page() {
	Pause_UI::Instance().enable(false);
	canvas_background->is_enabled = false;
	current_state = GameMenuState::in_GamePlay;
	button_click_sound.play();
}

void GameUIController::open_settings_page() {
	Pause_UI::Instance().enable(false);
	SettingsPage::Instance().enable(true);
	current_state = GameMenuState::in_GameSettings;
	button_click_sound.play();
}

void GameUIController::close_settings_page() {
	Pause_UI::Instance().enable(true);
	SettingsPage::Instance().enable(false);
	current_state = GameMenuState::in_GamePause;
	button_click_sound.play();
}

void GameUIController::show_HUD() {

}

void GameUIController::hide_HUD() {

}

void GameUIController::show_inventory() {

}

void GameUIController::hide_inventory() {

}

void GameUIController::notify_exit_to_main_menu() {
	should_exit_to_main_menu = true;
	button_click_sound.play();
}

void GameUIController::exit_to_main_menu() {
	should_exit_to_main_menu = false;
	current_state = GameMenuState::in_GamePlay;
	Pause_UI::Instance().enable(false);
	canvas_background->is_enabled = false;
	MenuUIController::Instance().start_world_saving();
	WorldManager::Instance().save_world();
}

void GameUIController::gameplay_update() {
	if (DeviceContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
		open_pause_page();
	}
	if (DeviceContext::keyBoard.key_is_pressed(Key::KeyEnter)) {
		HUD_UI::Instance().enable_console(true);
		current_state = GameMenuState::in_GamePlayConsole;
	}
}