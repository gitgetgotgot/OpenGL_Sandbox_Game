#include "Core/MainMenuUI/MenuUIController.h"
#include "Core/MainMenuUI/MainPage.h"
#include "Core/MainMenuUI/MainMenuPage.h"
#include "Core/MainMenuUI/SavesPage.h"
#include "Core/MainMenuUI/SettingsPage.h"
#include "Core/MainMenuUI/ModsPage.h"
#include "Core/MainMenuUI/CreatorPage.h"

#include "Core/WorldManager.h"
#include "Core/GameCore.h"

#include <IOSystem/SystemContext.h>
#include <thread>

void MenuUIController::init() {
	MainPage::Instance().setup_page();
	MainMenuPage::Instance().setup_page();
	SettingsPage::Instance().setup_page();
	SavesPage::Instance().setup_page();
	ModsPage::Instance().setup_page();
	CreatorPage::Instance().setup_page();

	MainPage::Instance().enable(true);
	MainMenuPage::Instance().enable(false);
	SettingsPage::Instance().enable(false);
	SavesPage::Instance().enable(false);
	ModsPage::Instance().enable(false);
	CreatorPage::Instance().enable(false);

	button_click_sound.setup_source("Core:Button");
	music_test.setup_source("Core:MusicTest");

	SavesPage::Instance().load_saves();
}

void MenuUIController::update() {
	MainPage::Instance().update();
	switch (current_state) {
	case MainMenuState::in_MainMenu:
		MainMenuPage::Instance().update();
		break;
	case MainMenuState::in_Saves:
		SavesPage::Instance().update();
		break;
	case MainMenuState::in_Creator:
		CreatorPage::Instance().update();
		break;
	case MainMenuState::in_Settings:
		SettingsPage::Instance().update();
		break;
	case MainMenuState::in_Mods:
		ModsPage::Instance().update();
		break;
	case MainMenuState::Generating_World:
		if (WorldManager::Instance().world_is_created()) {
			stop_world_generation();
			SavesPage::Instance().load_save(CreatorPage::Instance().get_last_created_save_name());
		}
		break;
	case MainMenuState::Loading_World:
		if (WorldManager::Instance().world_is_loaded()) {
			WorldManager::Instance().update_active_world_buffers();
			stop_world_loading();
		}
		break;
	case MainMenuState::Saving_World:
		if (WorldManager::Instance().world_is_saved()) {
			WorldManager::Instance().free_world_data();
			stop_world_saving();
		}
		break;
	}
}

void MenuUIController::reload_resources() {

}

void MenuUIController::start_loading_resources() {
	MainPage::Instance().show_tooltip_res_load();
}
void MenuUIController::stop_loading_resources() {
	MainPage::Instance().hide_tooltip_res_load();
}

void MenuUIController::start_world_generation() {
	CreatorPage::Instance().enable(false);
	current_state = MainMenuState::Generating_World;
	MainPage::Instance().show_tooltip_world_gen();
}
void MenuUIController::stop_world_generation() {
	SavesPage::Instance().enable(true);
	current_state = MainMenuState::in_Saves;
	MainPage::Instance().hide_tooltip_world_gen();
}

void MenuUIController::start_world_loading() {
	SavesPage::Instance().enable(false);
	current_state = MainMenuState::Loading_World;
	MainPage::Instance().show_tooltip_world_load();
}
void MenuUIController::stop_world_loading() {
	MainPage::Instance().enable(false);
	current_state = MainMenuState::in_MainMenu;
	MainPage::Instance().hide_tooltip_world_load();
	GameEngine::switch_update_state(GameEngine::Game_State::inGame);
}

void MenuUIController::start_world_saving() {
	current_state = MainMenuState::Saving_World;
	MainPage::Instance().enable(true);
	MainPage::Instance().show_tooltip_world_save();
	GameEngine::switch_update_state(GameEngine::Game_State::inMainMenu);
}
void MenuUIController::stop_world_saving() {
	MainMenuPage::Instance().enable(true);
	current_state = MainMenuState::in_MainMenu;
	MainPage::Instance().hide_tooltip_world_save();
}

void MenuUIController::load_saves() {
	SavesPage::Instance().load_saves();
}

void MenuUIController::load_mods() {
	ModsPage::Instance().load_mods_UI();
}

void MenuUIController::open_main_menu_page() {
	MainMenuPage::Instance().enable(true);
}

void MenuUIController::close_main_menu_page() {
	MainMenuPage::Instance().enable(false);
}

void MenuUIController::exit_game() {
	button_click_sound.play();
	SystemContext::exit_application();
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void MenuUIController::open_settings_page() {
	MainMenuPage::Instance().enable(false);
	SettingsPage::Instance().enable(true);
	button_click_sound.play();
}
void MenuUIController::close_settings_page() {
	MainMenuPage::Instance().enable(true);
	SettingsPage::Instance().enable(false);
	button_click_sound.play();
}

void MenuUIController::open_saves_page() {
	MainMenuPage::Instance().enable(false);
	SavesPage::Instance().enable(true);
	button_click_sound.play();
}
void MenuUIController::close_saves_page() {
	MainMenuPage::Instance().enable(true);
	SavesPage::Instance().enable(false);
	button_click_sound.play();
}

void MenuUIController::open_mods_page() {
	MainMenuPage::Instance().enable(false);
	ModsPage::Instance().enable(true);
	button_click_sound.play();
}
void MenuUIController::close_mods_page() {
	MainMenuPage::Instance().enable(true);
	ModsPage::Instance().enable(false);
	button_click_sound.play();
}

void MenuUIController::open_creator_page() {
	SavesPage::Instance().enable(false);
	CreatorPage::Instance().enable(true);
	button_click_sound.play();
}
void MenuUIController::close_creator_page() {
	SavesPage::Instance().enable(true);
	CreatorPage::Instance().enable(false);
	button_click_sound.play();
}