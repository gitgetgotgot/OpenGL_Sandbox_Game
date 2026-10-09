#include "Core/MainMenu.h"
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

void MainMenuManager::init() {
	setup_main_page();
	setup_settings_page();
	setup_saves_page();
	setup_mods_page();
	setup_creator_page();

	canvas_main->is_enabled = true;
	canvas_settings->is_enabled = false;
	canvas_saves->is_enabled = false;
	canvas_mods->is_enabled = false;
	canvas_creator->is_enabled = false;

	button_click_sound.setup_audio_source("Core:Button");
}

void MainMenuManager::update() {

}


void MainMenuManager::setup_main_page() {
	canvas_main = CoreUI::UI_Creator::create_canvas();

	uint32_t button_sprite_id = CoreResource::SpriteManager::get_instance().get_sprite9sliced_id("Core:Button").value();
	uint32_t sprite_white_id = CoreResource::SpriteManager::get_instance().get_sprite_id("Core:White").value();

	auto background = CoreUI::UI_Creator::create_image(
		sprite_white_id, glm::vec2(SystemContext::display.ratio * 2.0f, 2.0f), glm::vec2(0.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
	);

	auto button_play = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.4f), button_sprite_id,
		"Play", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_settings = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.275f), button_sprite_id,
		"Settings", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_mods = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.15f), button_sprite_id,
		"Mods", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_exit = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.025f), button_sprite_id,
		"Exit", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);

	auto menu_logo = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.8f), "Main Menu", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f
	);
	auto _menu_logo_text = menu_logo->get_component<CoreUI::SDF_Text>();
	_menu_logo_text->set_alignment(CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);

	auto version = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(0.05f, 0.1f), glm::vec2(-SystemContext::display.ratio + 0.05f, -0.95f), "Version 1.0.0", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.075f
	);

	button_play->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_saves_page>(&Instance());
	button_settings->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_settings_page>(&Instance());
	button_mods->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_mods_page>(&Instance());
	button_exit->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::exit_game>(&Instance());

	canvas_main->add_object(background);
	canvas_main->add_object(button_play);
	canvas_main->add_object(button_settings);
	canvas_main->add_object(button_mods);
	canvas_main->add_object(button_exit);
	canvas_main->add_object(menu_logo);
	canvas_main->add_object(version);
}

void MainMenuManager::setup_settings_page() {
	canvas_settings = CoreUI::UI_Creator::create_canvas();

	uint32_t button_sprite_id = CoreResource::SpriteManager::get_instance().get_sprite9sliced_id("Core:Button").value();
	uint32_t sprite_white_id = CoreResource::SpriteManager::get_instance().get_sprite_id("Core:White").value();

	auto background = CoreUI::UI_Creator::create_image(
		sprite_white_id, glm::vec2(SystemContext::display.ratio * 2.0f, 2.0f), glm::vec2(0.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
	);

	auto settings_logo = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.8f), "Settings", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f
	);
	settings_logo->get_component<CoreUI::SDF_Text>()->set_alignment(
		CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);

	auto button_resolution = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(1.5f, 0.1f), glm::vec2(0.0f, 0.545f), button_sprite_id,
		"Resolution: 1920x1080", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_fullscreen = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(-0.4f, 0.42f), button_sprite_id,
		"Fullscreen: OFF", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_vsync = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.4f, 0.42f), button_sprite_id,
		"Vsync: ON", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_exit = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, -0.8f), button_sprite_id,
		"Save&Exit", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);

	auto scroll_master_volume = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(1.0f, 0.075f), glm::vec2(1.0f, 0.025f), glm::vec2(0.075f, 0.075f), glm::vec2(0.3f, 0.295f),
		false, true, sprite_white_id, button_sprite_id, glm::vec4(0.0f, 0.5f, 0.9f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), false
	);
	auto scroll_music_volume = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(1.0f, 0.075f), glm::vec2(1.0f, 0.025f), glm::vec2(0.075f, 0.075f), glm::vec2(0.3f, 0.17f),
		false, true, sprite_white_id, button_sprite_id, glm::vec4(0.0f, 0.5f, 0.9f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), false
	);
	auto scroll_sfx_volume = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(1.0f, 0.075f), glm::vec2(1.0f, 0.025f), glm::vec2(0.075f, 0.075f), glm::vec2(0.3f, 0.045f),
		false, true, sprite_white_id, button_sprite_id, glm::vec4(0.0f, 0.5f, 0.9f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), false
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

	SettingsManager::Instance().resolution_text = CoreUI::UI_ObjectManager::get_instance().get(
		button_resolution->transform.children[1])->get_component<CoreUI::SDF_Text>();
	SettingsManager::Instance().fullscreen_text = CoreUI::UI_ObjectManager::get_instance().get(
		button_fullscreen->transform.children[1])->get_component<CoreUI::SDF_Text>();
	SettingsManager::Instance().vsync_text = CoreUI::UI_ObjectManager::get_instance().get(
		button_vsync->transform.children[1])->get_component<CoreUI::SDF_Text>();

	auto master_volume_slider = scroll_master_volume->get_component<CoreUI::ScrollBar>();
	auto music_volume_slider = scroll_music_volume->get_component<CoreUI::ScrollBar>();
	auto sfx_volume_slider = scroll_sfx_volume->get_component<CoreUI::ScrollBar>();
	SettingsManager::Instance().volume_slider_master = master_volume_slider;
	SettingsManager::Instance().volume_slider_music = music_volume_slider;
	SettingsManager::Instance().volume_slider_sfx = sfx_volume_slider;
	master_volume_slider->on_value_changed.set_callback_method<SettingsManager, &SettingsManager::set_master_volume>(&SettingsManager::Instance());

	button_resolution->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SettingsManager, &SettingsManager::change_resolution>(&SettingsManager::Instance());
	button_fullscreen->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SettingsManager, &SettingsManager::toggle_fullscreen>(&SettingsManager::Instance());
	button_vsync->get_component<CoreUI::Button>()->on_button_click.set_callback_method<SettingsManager, &SettingsManager::toggle_vsync>(&SettingsManager::Instance());
	button_exit->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_settings_page>(&Instance());

	canvas_settings->add_object(background);
	canvas_settings->add_object(settings_logo);
	canvas_settings->add_object(button_resolution);
	canvas_settings->add_object(button_fullscreen);
	canvas_settings->add_object(button_vsync);
	canvas_settings->add_object(scroll_master_volume);
	canvas_settings->add_object(scroll_music_volume);
	canvas_settings->add_object(scroll_sfx_volume);
	canvas_settings->add_object(master_volume_text);
	canvas_settings->add_object(music_volume_text);
	canvas_settings->add_object(sfx_volume_text);
	canvas_settings->add_object(button_exit);
}

void MainMenuManager::setup_saves_page() {
	canvas_saves = CoreUI::UI_Creator::create_canvas();

	uint32_t tooltip_sprite_id = CoreResource::SpriteManager::get_instance().get_sprite9sliced_id("Core:Tooltip").value();
	uint32_t button_sprite_id = CoreResource::SpriteManager::get_instance().get_sprite9sliced_id("Core:Button").value();
	uint32_t sprite_white_id = CoreResource::SpriteManager::get_instance().get_sprite_id("Core:White").value();

	auto background = CoreUI::UI_Creator::create_image(
		sprite_white_id, glm::vec2(SystemContext::display.ratio * 2.0f, 2.0f), glm::vec2(0.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
	);

	auto button_back = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.9f, 0.1f), glm::vec2(-0.5f, -0.8f), button_sprite_id,
		"Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	auto button_create = CoreUI::UI_Creator::create_button_image9sliced(
		glm::vec2(0.9f, 0.1f), glm::vec2(0.5f, -0.8f), button_sprite_id,
		"Create", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);

	button_back->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_saves_page>(&Instance());
	button_create->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::open_creator_page>(&Instance());

	auto saves_panel = CoreUI::UI_Creator::create_panel(
		glm::vec2(2.0f, 1.4f), glm::vec2(0.0f), sprite_white_id, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f)
	);

	auto saves_scroll_view = CoreUI::UI_Creator::create_scroll_view(
		glm::vec2(2.0f, 1.4f), glm::vec2(0.0f)
	);
	auto scroll_view = saves_scroll_view->get_component<CoreUI::ScrollView>();
	scroll_view->set_group_layout(CoreUI::ScrollView::GroupLayout::LAYOUT_VERTICAL);

	for (int i = 0; i < 30; ++i) {
		auto save_image_obj = CoreUI::UI_Creator::create_image9sliced(
			tooltip_sprite_id, glm::vec2(1.9f, 0.35f), glm::vec2(0.0f), glm::vec4(1.0f)
		);

		saves_scroll_view.add_child(save_image_obj);
	}
	saves_panel.add_child(saves_scroll_view);

	canvas_saves->add_object(background);
	canvas_saves->add_object(button_back);
	canvas_saves->add_object(button_create);
	canvas_saves->add_object(saves_panel);

	auto scroll_bar_Y = CoreUI::UI_Creator::create_scroll_bar(
		glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 1.4f), glm::vec2(0.08f, 0.08f), glm::vec2(0.96f, 0.0f),
		false, true, sprite_white_id, button_sprite_id, glm::vec4(0.0f, 0.3f, 0.7f, 1.0f), glm::vec4(0.0f, 0.0f, 0.6f, 1.0f), true
	);
	auto scrollbar = scroll_bar_Y->get_component<CoreUI::ScrollBar>();
	scroll_view->has_vertical_scrollbar = true;
	scroll_view->bind_vertical_scrollbar(scrollbar);

	canvas_saves->add_object(scroll_bar_Y);
}

void MainMenuManager::setup_mods_page() {
	canvas_mods = CoreUI::UI_Creator::create_canvas();
}

void MainMenuManager::setup_creator_page() {
	canvas_creator = CoreUI::UI_Creator::create_canvas();
}


void MainMenuManager::exit_game() {
	SystemContext::exit_application();
}

void MainMenuManager::open_main_page() {
	canvas_main->is_enabled = true;
}
void MainMenuManager::close_main_page() {
	canvas_main->is_enabled = false;
}

void MainMenuManager::open_settings_page() {
	canvas_main->is_enabled = false;
	canvas_settings->is_enabled = true;
	button_click_sound.play(true);
}
void MainMenuManager::close_settings_page() {
	canvas_main->is_enabled = true;
	canvas_settings->is_enabled = false;
	button_click_sound.stop();
}

void MainMenuManager::open_saves_page() {
	canvas_main->is_enabled = false;
	canvas_saves->is_enabled = true;
	button_click_sound.play();
}
void MainMenuManager::close_saves_page() {
	canvas_main->is_enabled = true;
	canvas_saves->is_enabled = false;
	button_click_sound.play();
}

void MainMenuManager::open_mods_page() {
	canvas_main->is_enabled = false;
	canvas_mods->is_enabled = true;
	button_click_sound.play();
}
void MainMenuManager::close_mods_page() {
	canvas_main->is_enabled = true;
	canvas_mods->is_enabled = false;
	button_click_sound.play();
}

void MainMenuManager::open_creator_page() {
	canvas_saves->is_enabled = false;
	canvas_saves->is_enabled = true;
	button_click_sound.play();
}
void MainMenuManager::close_creator_page() {
	canvas_saves->is_enabled = true;
	canvas_saves->is_enabled = false;
	button_click_sound.play();
}