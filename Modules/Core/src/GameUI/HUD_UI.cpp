#include "Core/GameUI/HUD_UI.h"
#include <UI/UI_Creator.h>
#include <UI/SDF_Text.h>
#include <Resources/Sprite.h>

void HUD_UI::update() {

}

void HUD_UI::enable(bool state) {
	canvas->is_enabled = state;
}

void HUD_UI::setup() {
	canvas = Engine::UI::UI_Creator::create_canvas();

	uint32_t frame1_id = Engine::Resource::SpriteManager::Instance().get_sprite9sliced_id("Core:UI_Frame1").value();

	console_obj = Engine::UI::UI_Creator::create_input_field_image(
		glm::vec2(3.0f, 0.1f), glm::vec2(0.0f, -0.93), true, frame1_id, glm::vec4(1.0f), 0.06f, 0.02f, 0.02f
	);
	console_input_field = console_obj->get_component<Engine::UI::InputField>();
	console_obj->is_enabled = false;

	canvas->add_object(console_obj);
}

void HUD_UI::enable_console(bool state) {
	console_obj->is_enabled = state;
}