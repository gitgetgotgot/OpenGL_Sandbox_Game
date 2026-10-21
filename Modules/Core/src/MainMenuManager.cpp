#include "Core/MainMenuManager.h"
#include "Core/SettingsManager.h"
#include <UI/UI_Creator.h>
#include <UI/Button.h>
#include <UI/SDF_Text.h>
#include <UI/ScrollView.h>
#include <UI/ScrollBar.h>
#include <UI/UI_ObjectManager.h>
#include <Resources/Sprite.h>
#include <IOSystem/SystemContext.h>
#include <filesystem>
#include <Audio/AudioSystem.h>
#include <Utility/TimeManager.h>
#include <thread>

void MainMenuManager::init() {
	main_page.setup_page();
	main_menu_page.setup_page();
	settings_page.setup_page();
	saves_page.setup_page();
	mods_page.setup_page();
	creator_page.setup_page();

	main_page.enable(true);
	main_menu_page.enable(false);
	settings_page.enable(false);
	saves_page.enable(false);
	mods_page.enable(false);
	creator_page.enable(false);

	button_click_sound.setup_source("Core:Button");
	music_test.setup_source("Core:MusicTest");

	saves_page.load_saves();
}

void MainMenuManager::update() {
	main_page.update();
}

void MainMenuManager::start_loading_resources() {
	main_page.start_loading_resources();
}

void MainMenuManager::stop_loading_resources() {
	main_page.stop_loading_resources();
}

void MainMenuManager::load_saves() {
	saves_page.load_saves();
}

void MainMenuManager::load_mods() {
	mods_page.load_mods_UI();
}

void MainMenuManager::open_main_menu_page() {
	main_menu_page.enable(true);
}

void MainMenuManager::close_main_menu_page() {
	main_menu_page.enable(false);
}

void MainMenuManager::exit_game() {
	button_click_sound.play();
	SystemContext::exit_application();
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void MainMenuManager::open_settings_page() {
	main_menu_page.enable(false);
	settings_page.enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_settings_page() {
	main_menu_page.enable(true);
	settings_page.enable(false);
	button_click_sound.play();
}

void MainMenuManager::open_saves_page() {
	main_menu_page.enable(false);
	saves_page.enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_saves_page() {
	main_menu_page.enable(true);
	saves_page.enable(false);
	button_click_sound.play();
}

void MainMenuManager::open_mods_page() {
	main_menu_page.enable(false);
	mods_page.enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_mods_page() {
	main_menu_page.enable(true);
	mods_page.enable(false);
	button_click_sound.play();
}

void MainMenuManager::open_creator_page() {
	saves_page.enable(false);
	creator_page.enable(true);
	button_click_sound.play();
}
void MainMenuManager::close_creator_page() {
	saves_page.enable(true);
	creator_page.enable(false);
	button_click_sound.play();
}