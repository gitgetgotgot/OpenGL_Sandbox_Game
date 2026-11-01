#pragma once
#include <UI/Canvas.h>
#include <UI/InputField.h>
#include <UI/SDF_Text.h>
#include <UI/Image.h>

class HUD_UI {
public:
	static HUD_UI& Instance() {
		static HUD_UI h;
		return h;
	}
	void update();
	void enable(bool state);
	void setup();
	void enable_console(bool state);
private:
	HUD_UI() = default;
	~HUD_UI() = default;

	Engine::UI::UI_Canvas_Ptr canvas;
	Engine::UI::UI_Component_Ptr<Engine::UI::Image> hotbar_slot_item[10];
	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> hotbar_text[10];
	Engine::UI::UI_Component_Ptr<Engine::UI::SDF_Text> active_item_hint_text;
	Engine::UI::UI_Obj_Ptr console_obj;
	Engine::UI::UI_Component_Ptr<Engine::UI::InputField> console_input_field;
};