#include <IOSystem/SystemContext.h>
#include "Core/Menu/SettingsPage.h"
#include <Core/MainMenuManager.h>
#include <UI/Button.h>
#include <Core/SettingsManager.h>
#include <Resources/Sprite.h>
#include <UI/UI_Creator.h>
#include <UI/UI_ObjectManager.h>

void SettingsPage::update() {

}

void SettingsPage::enable(bool state) {
	canvas->is_enabled = state;
}

void SettingsPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	button_click_sound.setup_source("Core:Button");

	uint32_t sprite_white_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:White").value();
	uint32_t sprite_gradient_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:Gradient").value();
	uint32_t slider_sprite_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p").value();
	uint32_t button64_sprite_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();

	auto settings_logo = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.8f), "Settings", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f
	);
	settings_logo->get_component<CoreUI::SDF_Text>()->set_alignment(
		CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);

	auto button_resolution = CoreUI::UI_Creator::create_button_image(
		glm::vec2(1.5f, 0.1f), glm::vec2(0.0f, 0.545f), true, button64_sprite_id,
		true, "", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_fullscreen = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(-0.4f, 0.42f), true, button64_sprite_id,
		true, "", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_vsync = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.4f, 0.42f), true, button64_sprite_id,
		true, "", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_exit = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, -0.8f), true, button64_sprite_id,
		true, "Save&Exit", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);

	auto scroll_master_volume = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(1.0f, 0.075f), glm::vec2(1.0f, 0.025f), glm::vec2(0.075f, 0.075f), glm::vec2(0.3f, 0.295f),
		false, true, sprite_gradient_id, slider_sprite_id, glm::vec4(0.0f, 0.5f, 0.9f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		false, true, true, 100
	);
	auto scroll_music_volume = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(1.0f, 0.075f), glm::vec2(1.0f, 0.025f), glm::vec2(0.075f, 0.075f), glm::vec2(0.3f, 0.17f),
		false, true, sprite_gradient_id, slider_sprite_id, glm::vec4(0.0f, 0.5f, 0.9f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		false, true, true, 100
	);
	auto scroll_sfx_volume = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(1.0f, 0.075f), glm::vec2(1.0f, 0.025f), glm::vec2(0.075f, 0.075f), glm::vec2(0.3f, 0.045f),
		false, true, sprite_gradient_id, slider_sprite_id, glm::vec4(0.0f, 0.5f, 0.9f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		false, true, true, 100
	);

	auto master_volume_text = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(-0.3f, 0.295f), "Master Volume:", glm::vec4(1.0f), 0.07f
	);
	auto music_volume_text = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(-0.3f, 0.17f), "Music Volume:", glm::vec4(1.0f), 0.07f
	);
	auto sfx_volume_text = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(-0.3f, 0.045f), "SFX Volume:", glm::vec4(1.0f), 0.07f
	);

	resolution_text = CoreUI::UI_ObjectManager::get_instance().get(
		button_resolution->transform.children[1])->get_component<CoreUI::SDF_Text>();
	fullscreen_text = CoreUI::UI_ObjectManager::get_instance().get(
		button_fullscreen->transform.children[1])->get_component<CoreUI::SDF_Text>();
	vsync_text = CoreUI::UI_ObjectManager::get_instance().get(
		button_vsync->transform.children[1])->get_component<CoreUI::SDF_Text>();

	auto master_volume_slider = scroll_master_volume->get_component<CoreUI::ScrollBar>();
	auto music_volume_slider = scroll_music_volume->get_component<CoreUI::ScrollBar>();
	auto sfx_volume_slider = scroll_sfx_volume->get_component<CoreUI::ScrollBar>();
	volume_slider_master = master_volume_slider;
	volume_slider_music = music_volume_slider;
	volume_slider_sfx = sfx_volume_slider;
	master_volume_slider->on_value_changed.set_callback_method<SettingsPage, &SettingsPage::set_master_volume>(this);
	music_volume_slider->on_value_changed.set_callback_method<SettingsPage, &SettingsPage::set_music_volume>(this);
	sfx_volume_slider->on_value_changed.set_callback_method<SettingsPage, &SettingsPage::set_sfx_volume>(this);

	button_resolution->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SettingsPage, &SettingsPage::change_resolution>(this);
	button_fullscreen->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SettingsPage, &SettingsPage::toggle_fullscreen>(this);
	button_vsync->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SettingsPage, &SettingsPage::toggle_vsync>(this);
	button_exit->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_settings_page>(&MainMenuManager::Instance());

	canvas->add_object(settings_logo);
	canvas->add_object(button_resolution);
	canvas->add_object(button_fullscreen);
	canvas->add_object(button_vsync);
	canvas->add_object(scroll_master_volume);
	canvas->add_object(scroll_music_volume);
	canvas->add_object(scroll_sfx_volume);
	canvas->add_object(master_volume_text);
	canvas->add_object(music_volume_text);
	canvas->add_object(sfx_volume_text);
	canvas->add_object(button_exit);

	DisplayController::DisplayVideoMode& mode = SystemContext::display.get_current_video_mode();
	text_builder.add_text("Resolution: ").add_int(mode.width).add_text("x").add_int(mode.height);
	resolution_text->set_text(text_builder.data());
	text_builder.reset();

	if (SettingsManager::Instance().is_fullscreen())
		fullscreen_text->set_text("Fullscreen: ON");
	else
		fullscreen_text->set_text("Fullscreen: OFF");

	if (SettingsManager::Instance().is_vsync_on())
		vsync_text->set_text("Vsync: ON");
	else
		vsync_text->set_text("Vsync: OFF");

	volume_slider_master->set_value(SettingsManager::Instance().get_master_volume());
	volume_slider_music->set_value(SettingsManager::Instance().get_music_volume());
	volume_slider_sfx->set_value(SettingsManager::Instance().get_sfx_volume());
}

void SettingsPage::change_resolution() {
	button_click_sound.play();
	SettingsManager::Instance().change_resolution();
	DisplayController::DisplayVideoMode& mode = SystemContext::display.get_current_video_mode();
	text_builder.add_text("Resolution: ").add_int(mode.width).add_text("x").add_int(mode.height);
	resolution_text->set_text(text_builder.data());
	text_builder.reset();
}

void SettingsPage::toggle_fullscreen() {
	button_click_sound.play();
	SettingsManager::Instance().toggle_fullscreen();
	if (SettingsManager::Instance().is_fullscreen())
		fullscreen_text->set_text("Fullscreen: ON");
	else
		fullscreen_text->set_text("Fullscreen: OFF");
}

void SettingsPage::toggle_vsync() {
	button_click_sound.play();
	SettingsManager::Instance().toggle_vsync();
	if (SettingsManager::Instance().is_vsync_on())
		vsync_text->set_text("Vsync: ON");
	else
		vsync_text->set_text("Vsync: OFF");
}

void SettingsPage::set_master_volume(float volume) {
	SettingsManager::Instance().set_master_volume(volume);
}

void SettingsPage::set_music_volume(float volume) {
	SettingsManager::Instance().set_music_volume(volume);
}

void SettingsPage::set_sfx_volume(float volume) {
	SettingsManager::Instance().set_sfx_volume(volume);
}