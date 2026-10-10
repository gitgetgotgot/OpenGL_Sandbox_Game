#include "Core/SettingsManager.h"
#include <IOSystem/SystemContext.h>
#include <Audio/AudioSystem.h>

void SettingsManager::load_settings() {
	mixer_ui.setup_mixer("Core:UI");
	mixer_music.setup_mixer("Core:Music");
	mixer_sfx.setup_mixer("Core:SFX");
}

void SettingsManager::change_resolution() {
	SystemContext::display.change_resolution(true);
	DisplayController::Resolution& res = SystemContext::display.get_current_video_mode();
	text_builder.add_text("Resolution: ").add_int(res.width).add_text("x").add_int(res.height);
	resolution_text->set_text(text_builder.data());
	text_builder.reset();
}

void SettingsManager::toggle_fullscreen() {
	fullscreen = !fullscreen;
	SystemContext::display.toggle_fullscreen(fullscreen);
	if (fullscreen)
		fullscreen_text->set_text("Fullscreen: ON");
	else
		fullscreen_text->set_text("Fullscreen: OFF");
}

void SettingsManager::toggle_vsync() {
	vsync_on = !vsync_on;
	SystemContext::display.toggle_VSYNC(vsync_on);
	if (vsync_on)
		vsync_text->set_text("Vsync: ON");
	else
		vsync_text->set_text("Vsync: OFF");
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