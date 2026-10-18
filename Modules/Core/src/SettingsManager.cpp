#include "Core/SettingsManager.h"
#include <IOSystem/SystemContext.h>
#include <Audio/AudioSystem.h>
#include <Utility/DataParser.h>

void SettingsManager::load_settings() {
	mixer_ui.setup_mixer("Core:UI");
	mixer_music.setup_mixer("Core:Music");
	mixer_sfx.setup_mixer("Core:SFX");

	std::filesystem::path settings_file_path = RESOURCE_ROOT;
	settings_file_path /= "game_settings.json";
	if (!std::filesystem::exists(settings_file_path)) {
		throw std::runtime_error("Fatal Error, Couldn't load settings file!");
	}
	DataParser parser;
	parser.parse_JSON_format(settings_file_path);
	const auto& screen = parser.node_root["screen"];
	const bool fullscreen = screen["fullscreen"].get_as<bool>();
	const bool vsync = screen["vsync"].get_as<bool>();
	const auto& res = screen["resolution"];
	const uint32_t width = res["width"].get_as<uint32_t>();
	const uint32_t height = res["height"].get_as<uint32_t>();

	SystemContext::display.toggle_fullscreen(fullscreen);
	this->fullscreen = fullscreen;

	SystemContext::display.toggle_VSYNC(vsync);
	this->vsync_on = vsync;

	SystemContext::display.change_resolution(width, height);

	const auto& volume = parser.node_root["volume"];
	const uint32_t master_v = volume["master"].get_as<uint32_t>();
	const uint32_t music_v = volume["music"].get_as<uint32_t>();
	const uint32_t sfx_v = volume["sfx"].get_as<uint32_t>();
	set_master_volume((float)master_v / 100.0f);
	set_music_volume((float)music_v / 100.0f);
	set_sfx_volume((float)sfx_v / 100.0f);
}

void SettingsManager::change_resolution() {
	SystemContext::display.change_resolution(true);
}

void SettingsManager::toggle_fullscreen() {
	fullscreen = !fullscreen;
	SystemContext::display.toggle_fullscreen(fullscreen);
}

void SettingsManager::toggle_vsync() {
	vsync_on = !vsync_on;
	SystemContext::display.toggle_VSYNC(vsync_on);
}

void SettingsManager::set_master_volume(float volume) {
	CoreAudio::AudioSystem::Instance().set_master_volume(volume);
}

void SettingsManager::set_music_volume(float volume) {
	mixer_music.set_volume(volume);
}

void SettingsManager::set_sfx_volume(float volume) {
	mixer_sfx.set_volume(volume);
}

float SettingsManager::get_master_volume() {
	return CoreAudio::AudioSystem::Instance().get_master_volume();
}

float SettingsManager::get_music_volume() {
	return mixer_music.get_volume();
}

float SettingsManager::get_sfx_volume() {
	return mixer_sfx.get_volume();
}