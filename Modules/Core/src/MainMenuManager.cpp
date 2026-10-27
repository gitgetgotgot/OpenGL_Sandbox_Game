#include "Core/MainMenuManager.h"
#include "Core/Menu/MainPage.h"
#include "Core/Menu/MainMenuPage.h"
#include "Core/Menu/SavesPage.h"
#include "Core/Menu/SettingsPage.h"
#include "Core/Menu/ModsPage.h"
#include "Core/Menu/CreatorPage.h"

#include "Core/WorldManager.h"

#include "Core/GameCore.h"

#include <IOSystem/SystemContext.h>
#include <thread>

void MainMenuManager::init() {
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

void MainMenuManager::update() {
	MainPage::Instance().update();
	switch (current_state) {
	case MenuCurrentState::in_MainMenu:
		MainMenuPage::Instance().update();
		break;
	case MenuCurrentState::in_Saves:
		SavesPage::Instance().update();
		break;
	case MenuCurrentState::in_Creator:
		CreatorPage::Instance().update();
		break;
	case MenuCurrentState::in_Settings:
		SettingsPage::Instance().update();
		break;
	case MenuCurrentState::in_Mods:
		ModsPage::Instance().update();
		break;
	case MenuCurrentState::Generating_World:
		if (WorldManager::Instance().world_is_created()) {
			stop_world_generation();
		}
		break;
	case MenuCurrentState::Loading_World:
		if (WorldManager::Instance().world_is_loaded()) {
			WorldManager::Instance().update_active_world_buffers();
			stop_world_loading();
			GameEngine::switch_update_state(GameEngine::Game_State::inWorld);
		}
		break;
	}
}

void MainMenuManager::reload_resources() {

}

void MainMenuManager::start_loading_resources() {
	MainPage::Instance().show_tooltip_res_load();
}
void MainMenuManager::stop_loading_resources() {
	MainPage::Instance().hide_tooltip_res_load();
}

void MainMenuManager::start_world_generation() {
	CreatorPage::Instance().enable(false);
	current_state = MenuCurrentState::Generating_World;
	MainPage::Instance().show_tooltip_world_gen();
}
void MainMenuManager::stop_world_generation() {
	SavesPage::Instance().enable(true);
	current_state = MenuCurrentState::in_Saves;
	MainPage::Instance().hide_tooltip_world_gen();
}

void MainMenuManager::start_world_loading() {
	SavesPage::Instance().enable(false);
	current_state = MenuCurrentState::Loading_World;
	MainPage::Instance().show_tooltip_world_load();
}
void MainMenuManager::stop_world_loading() {
	MainPage::Instance().enable(false);
	current_state = MenuCurrentState::in_MainMenu;
	MainPage::Instance().hide_tooltip_world_load();
}

void MainMenuManager::load_saves() {
	SavesPage::Instance().load_saves();
}

void MainMenuManager::load_mods() {
	ModsPage::Instance().load_mods_UI();
}

void MainMenuManager::open_main_menu_page() {
	MainMenuPage::Instance().enable(true);
}

void MainMenuManager::close_main_menu_page() {
	MainMenuPage::Instance().enable(false);
}

void MainMenuManager::exit_game() {
	button_click_sound.play();
	SystemContext::exit_application();
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void MainMenuManager::open_settings_page() {
	MainMenuPage::Instance().enable(false);
	SettingsPage::Instance().enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_settings_page() {
	MainMenuPage::Instance().enable(true);
	SettingsPage::Instance().enable(false);
	button_click_sound.play();
}

void MainMenuManager::open_saves_page() {
	MainMenuPage::Instance().enable(false);
	SavesPage::Instance().enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_saves_page() {
	MainMenuPage::Instance().enable(true);
	SavesPage::Instance().enable(false);
	button_click_sound.play();
}

void MainMenuManager::open_mods_page() {
	MainMenuPage::Instance().enable(false);
	ModsPage::Instance().enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_mods_page() {
	MainMenuPage::Instance().enable(true);
	ModsPage::Instance().enable(false);
	button_click_sound.play();
}

void MainMenuManager::open_creator_page() {
	SavesPage::Instance().enable(false);
	CreatorPage::Instance().enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_creator_page() {
	SavesPage::Instance().enable(true);
	CreatorPage::Instance().enable(false);
	button_click_sound.play();
}