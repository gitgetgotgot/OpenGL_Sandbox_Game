#include "UI/Button.h"

void CoreUI::Button::bind_components(
	UI_Component_Ptr<Image>& button_image,
	UI_Component_Ptr<SDF_Text>& button_text
) {
	this->button_image = button_image;
	this->button_text = button_text;
	button_image->set_color(color_base);
}

void CoreUI::Button::set_color_base(glm::vec4 color) {
	color_base = color;
}

void CoreUI::Button::set_color_hover(glm::vec4 color) {
	color_hover = color;
}

void CoreUI::Button::set_color_press(glm::vec4 color) {
	color_press = color;
}

void CoreUI::Button::_on_cursor_enter() {
	button_image->set_color(color_hover);
	on_cursor_enter();
}

void CoreUI::Button::_on_cursor_exit() {
	button_image->set_color(color_base);
	on_cursor_exit();
}

void CoreUI::Button::_on_cursor_pressed() {
	button_image->set_color(color_press);
}

void CoreUI::Button::_on_callback_use() {
	button_image->set_color(color_hover);
	on_button_click();
}