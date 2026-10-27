#pragma once
#include <UI/Canvas.h>
#include <UI/Button.h>
#include <UI/InputField.h>
#include <Audio/AudioSource.h>

class CreatorPage {
public:
	static CreatorPage& Instance() {
		static CreatorPage page;
		return page;
	}
	void update();
	void enable(bool state);
	void setup_page();
	void start_world_creation();
private:
	CreatorPage() = default;
	~CreatorPage() = default;

	void _choose_world_size_1();
	void _choose_world_size_2();
	void _choose_world_size_3();
	void _choose_world_size_4();

	Engine::Audio::AudioSource button_click_sound;
	Engine::UI::UI_Canvas_Ptr canvas;
	Engine::UI::UI_Component_Ptr<Engine::UI::InputField> world_input_field;
	Engine::UI::UI_Component_Ptr<Engine::UI::Button> world_size_buttons[4];
	uint32_t regular_button_sprite_id = 0;
	uint32_t chosen_button_sprite_id = 0;
	uint8_t world_size_id = 0; // small size by default
};