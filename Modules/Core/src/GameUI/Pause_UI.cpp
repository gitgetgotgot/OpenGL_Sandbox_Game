#include "Core/GameUI/Pause_UI.h"
#include "Core/GameUI/GameUIController.h"
#include <UI/UI_Creator.h>
#include <UI/SDF_Text.h>
#include <UI/Button.h>
#include <Resources/Sprite.h>
#include <IOSystem/SystemContext.h>

void Pause_UI::update() {
	if (SystemContext::keyBoard.key_is_pressed(Key::KeyEscape)) {
		GameUIController::Instance().close_pause_page();
	}
}

void Pause_UI::enable(bool state) {
	canvas->is_enabled = state;
}

void Pause_UI::setup() {
	canvas = Engine::UI::UI_Creator::create_canvas();

	uint32_t button64_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Button64p1").value();
	uint32_t frame0_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame0").value();

	auto background_obj = Engine::UI::UI_Creator::create_image9sliced(
		frame0_id, glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.4f), glm::vec4(1.0f)
	);

	auto pause_label = Engine::UI::UI_Creator::create_SDF_text(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.525f), "Pause", glm::vec4(1.0f), 0.08f
	);
	pause_label->get_component<Engine::UI::SDF_Text>()->set_alignment(Engine::UI::SDF_Text::TextHorizAlign::Center_Align, Engine::UI::SDF_Text::TextVertAlign::Middle_Align);

	auto button_continue = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.4f), true, button64_id,
		true, "Continue", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	button_continue->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<GameUIController, &GameUIController::close_pause_page>(&GameUIController::Instance());
	auto button_settings = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.275f), true, button64_id,
		true, "Settings", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	auto button_exit = Engine::UI::UI_Creator::create_button_image(
		glm::vec2(0.7f, 0.1f), glm::vec2(0.0f, 0.025f), true, button64_id,
		true, "Save&Exit", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), 0.08f
	);
	button_exit->get_component<Engine::UI::Button>()->on_button_click.set_callback_method<GameUIController, &GameUIController::exit_to_main_menu>(&GameUIController::Instance());

	canvas->add_object(background_obj);
	canvas->add_object(pause_label);
	canvas->add_object(button_continue);
	canvas->add_object(button_settings);
	canvas->add_object(button_exit);
}