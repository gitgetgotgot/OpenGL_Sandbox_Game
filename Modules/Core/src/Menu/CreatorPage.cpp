#include "Core/MainMenuManager.h"
#include "Core/Menu/CreatorPage.h"
#include "Core/Menu/SavesPage.h"
#include "Core/WorldManager.h"
#include <IOSystem/SystemContext.h>
#include <UI/UI_Creator.h>
#include <Resources/Sprite.h>

void CreatorPage::update() {
	if (SystemContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
		MainMenuManager::Instance().close_creator_page();
	}
}

void CreatorPage::enable(bool state) {
	canvas->is_enabled = state;
}

void CreatorPage::setup_page() {
	canvas = CoreUI::UI_Creator::create_canvas();

	button_click_sound.setup_source("Core:Button");

	regular_button_sprite_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	chosen_button_sprite_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p2").value();

	uint32_t gradient2_id = CoreResource::SpriteManager::Instance().get_sprite_id("Core:Gradient2").value();
	uint32_t frame0_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame0").value();
	uint32_t frame1_id = CoreResource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame1").value();

	auto background_obj = CoreUI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(2.12f, 1.52f), glm::vec2(0.0f), glm::vec4(1.0f)
	);
	background_obj->get_component<CoreUI::Image>()->set_border_size(0.06f);

	auto world_name_label = CoreUI::UI_Creator::create_SDF_text(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, 0.64), "World Name:", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.06f
	);
	world_name_label->get_component<CoreUI::SDF_Text>()->set_alignment(CoreUI::SDF_Text::TextHorizAlign::Center_Align, CoreUI::SDF_Text::TextVertAlign::Middle_Align);

	auto world_input_field_obj = CoreUI::UI_Creator::create_input_field_image(
		glm::vec2(1.25f, 0.1f), glm::vec2(0.0f, 0.54), true, frame1_id, glm::vec4(1.0f), 0.06f, 0.02f, 0.02f
	);
	world_input_field = world_input_field_obj->get_component<CoreUI::InputField>();

	auto button_option_1 = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.45f, 0.1f), glm::vec2(-0.75f, 0.35f), true, chosen_button_sprite_id,
		true, "Small", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.06f
	);
	auto button_option_2 = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.45f, 0.1f), glm::vec2(-0.25f, 0.35f), true, regular_button_sprite_id,
		true, "Medium", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.06f
	);
	auto button_option_3 = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.45f, 0.1f), glm::vec2(0.25f, 0.35f), true, regular_button_sprite_id,
		true, "Large", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.06f
	);
	auto button_option_4 = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.45f, 0.1f), glm::vec2(0.75f, 0.35f), true, regular_button_sprite_id,
		true, "Large++", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 0.06f
	);

	world_size_buttons[0] = button_option_1->get_component<CoreUI::Button>();
	world_size_buttons[1] = button_option_2->get_component<CoreUI::Button>();
	world_size_buttons[2] = button_option_3->get_component<CoreUI::Button>();
	world_size_buttons[3] = button_option_4->get_component<CoreUI::Button>();
	world_size_buttons[0]->on_button_click.set_callback_method<CreatorPage, &CreatorPage::_choose_world_size_1>(this);
	world_size_buttons[1]->on_button_click.set_callback_method<CreatorPage, &CreatorPage::_choose_world_size_2>(this);
	world_size_buttons[2]->on_button_click.set_callback_method<CreatorPage, &CreatorPage::_choose_world_size_3>(this);
	world_size_buttons[3]->on_button_click.set_callback_method<CreatorPage, &CreatorPage::_choose_world_size_4>(this);

	auto button_create = CoreUI::UI_Creator::create_button_image(
		glm::vec2(0.48f, 0.1f), glm::vec2(0.0f, 0.0f), true, regular_button_sprite_id,
		true, "Create", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.08f
	);
	button_create->get_component<CoreUI::Button>()->on_button_click.set_callback_method<CreatorPage, &CreatorPage::start_world_creation>(this);

	auto button_back = CoreUI::UI_Creator::create_button_image(
		glm::vec2(1.0f, 0.1f), glm::vec2(0.0f, -0.85f), true, regular_button_sprite_id,
		true, "Back", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.07f
	);
	button_back->get_component<CoreUI::Button>()->on_button_click.set_callback_method<MainMenuManager, &MainMenuManager::close_creator_page>(&MainMenuManager::Instance());

	canvas->add_object(background_obj);
	canvas->add_object(world_name_label);
	canvas->add_object(world_input_field_obj);
	canvas->add_object(button_option_1);
	canvas->add_object(button_option_2);
	canvas->add_object(button_option_3);
	canvas->add_object(button_option_4);
	canvas->add_object(button_create);
	canvas->add_object(button_back);
}

void CreatorPage::start_world_creation() {
	button_click_sound.play();
	std::string& input_text = world_input_field->field_text->text;
	if (input_text.empty()) {
		std::cout << "[WorldCreator]: Can't create world with empty WorldName!\n";
		return;
	}
	if (SavesPage::Instance().world_save_exists(input_text)) {
		std::cout << "[WorldCreator]: World with such WorldName already exists!\n";
		return;
	}

	World::WorldOptions world_options{};
	if (world_size_id == 0)
		world_options.size = World::WorldOptions::WorldSize::SMALL;
	else if (world_size_id == 1)
		world_options.size = World::WorldOptions::WorldSize::MEDIUM;
	else if (world_size_id == 2)
		world_options.size = World::WorldOptions::WorldSize::LARGE;
	else if (world_size_id == 3)
		world_options.size = World::WorldOptions::WorldSize::LARGE_PLUS_PLUS;

	MainMenuManager::Instance().start_world_generation();
	WorldManager::Instance().create_world(input_text, world_options);
}

void CreatorPage::_choose_world_size_1() {
	button_click_sound.play();
	if (world_size_id == 0) return;
	world_size_buttons[0]->button_image->set_sprite(chosen_button_sprite_id);
	world_size_buttons[world_size_id]->button_image->set_sprite(regular_button_sprite_id);
	world_size_id = 0;
}
void CreatorPage::_choose_world_size_2() {
	button_click_sound.play();
	if (world_size_id == 1) return;
	world_size_buttons[1]->button_image->set_sprite(chosen_button_sprite_id);
	world_size_buttons[world_size_id]->button_image->set_sprite(regular_button_sprite_id);
	world_size_id = 1;
}
void CreatorPage::_choose_world_size_3() {
	button_click_sound.play();
	if (world_size_id == 2) return;
	world_size_buttons[2]->button_image->set_sprite(chosen_button_sprite_id);
	world_size_buttons[world_size_id]->button_image->set_sprite(regular_button_sprite_id);
	world_size_id = 2;
}
void CreatorPage::_choose_world_size_4() {
	button_click_sound.play();
	if (world_size_id == 3) return;
	world_size_buttons[3]->button_image->set_sprite(chosen_button_sprite_id);
	world_size_buttons[world_size_id]->button_image->set_sprite(regular_button_sprite_id);
	world_size_id = 3;
}