#include "Core/SettingsManager.h"
#include <DeviceSystem/DeviceContext.h>
#include <Audio/AudioSystem.h>
#include <fstream>

void SettingsManager::load_settings() {
	mixer_ui.setup_mixer("Core:UI");
	mixer_music.setup_mixer("Core:Music");
	mixer_sfx.setup_mixer("Core:SFX");

	std::filesystem::path settings_file_path = "Config/game_config.json";
	if (!std::filesystem::exists(settings_file_path)) {
		throw std::runtime_error("Fatal Error, Couldn't load settings file!");
	}
	
	std::ifstream config_read(settings_file_path);
	nlohmann::json j;
	config_read >> j;

	config = j.get<Config>();

	DeviceContext::display.toggle_fullscreen(config.screen.fullscreen);
	DeviceContext::display.toggle_VSYNC(config.screen.vsync);

	DeviceContext::display.change_resolution(config.screen.width, config.screen.height);

	set_master_volume((float)config.volume.master / 100.0f);
	set_music_volume((float)config.volume.music / 100.0f);
	set_sfx_volume((float)config.volume.sfx / 100.0f);
}

void SettingsManager::change_resolution() {
	DeviceContext::display.change_resolution(true);
}

void SettingsManager::toggle_fullscreen() {
	config.screen.fullscreen = !config.screen.fullscreen;
	DeviceContext::display.toggle_fullscreen(config.screen.fullscreen);
}

void SettingsManager::toggle_vsync() {
	config.screen.vsync = !config.screen.vsync;
	DeviceContext::display.toggle_VSYNC(config.screen.vsync);
}

void SettingsManager::set_master_volume(float volume) {
	Engine::Audio::AudioSystem::Instance().set_master_volume(volume);
}

void SettingsManager::set_music_volume(float volume) {
	mixer_music.set_volume(volume);
}

void SettingsManager::set_sfx_volume(float volume) {
	mixer_sfx.set_volume(volume);
}

float SettingsManager::get_master_volume() {
	return Engine::Audio::AudioSystem::Instance().get_master_volume();
}

float SettingsManager::get_music_volume() {
	return mixer_music.get_volume();
}

float SettingsManager::get_sfx_volume() {
	return mixer_sfx.get_volume();
}