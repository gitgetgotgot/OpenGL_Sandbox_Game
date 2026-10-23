#include "Core/MainMenuManager.h"
#include "Core/Menu/MainPage.h"
#include "Core/Menu/MainMenuPage.h"
#include "Core/Menu/SavesPage.h"
#include "Core/Menu/SettingsPage.h"
#include "Core/Menu/ModsPage.h"
#include "Core/Menu/CreatorPage.h"

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
	switch (current_page) {
	case MenuCurrentPage::in_MainMenu:
		MainMenuPage::Instance().update();
		break;
	case MenuCurrentPage::in_Saves:
		SavesPage::Instance().update();
		break;
	case MenuCurrentPage::in_Creator:
		CreatorPage::Instance().update();
		break;
	case MenuCurrentPage::in_Settings:
		SettingsPage::Instance().update();
		break;
	case MenuCurrentPage::in_Mods:
		ModsPage::Instance().update();
		break;
	}
}

void MainMenuManager::start_loading_resources() {
	MainPage::Instance().start_loading_resources();
}

void MainMenuManager::stop_loading_resources() {
	MainPage::Instance().stop_loading_resources();
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