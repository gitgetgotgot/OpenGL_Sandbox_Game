#include "Core/GameUI/GameUIController.h"
#include "Core/GameUI/Pause_UI.h"
#include "Core/MainMenuUI/MenuUIController.h"

#include "Core/WorldManager.h"
#include <IOSystem/SystemContext.h>

void GameUIController::init() {
	Pause_UI::Instance().setup();
	Pause_UI::Instance().enable(false);
}

void GameUIController::update() {
	switch (current_state) {
	case GameMenuState::in_GamePlay:
		if (SystemContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
			open_pause_page();
		}
		break;
	case GameMenuState::in_GamePause:
		Pause_UI::Instance().update();
		break;
	}
}

void GameUIController::uninit() {

}

void GameUIController::open_pause_page() {
	Pause_UI::Instance().enable(true);
	current_state = GameMenuState::in_GamePause;
}

void GameUIController::close_pause_page() {
	Pause_UI::Instance().enable(false);
	current_state = GameMenuState::in_GamePlay;
}

void GameUIController::open_settings_page() {

}

void GameUIController::close_settings_page() {

}

void GameUIController::show_HUD() {

}

void GameUIController::hide_HUD() {

}

void GameUIController::show_inventory() {

}

void GameUIController::hide_inventory() {

}

void GameUIController::exit_to_main_menu() {
	current_state = GameMenuState::in_GamePlay;
	Pause_UI::Instance().enable(false);
	MenuUIController::Instance().start_world_saving();
	WorldManager::Instance().save_world();
}