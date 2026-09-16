#include "UI/InputField.h"
#include "UI/UI_ObjectManager.h"
#include <Utility/TimeManager.h>

void CoreUI::InputField::bind_components(
	UI_Component_Ptr<SDF_Text>& text,
	UI_Component_Ptr<Image>& cursor
) {
	text_comp = text;
	cursor_comp = cursor;
	cursor_comp->is_visible = false;
	text_comp->set_color(1.0f, 1.0f, 0.0f, 1.0f);
	float cursor_offset = text_comp->get_char_offset(cursor_position);
	cursor_initial_x_pos = -UI_ObjectManager::get_instance().get(text_comp.get_id())->transform.size.x * 0.5f;
	UI_ObjectManager::get_instance().get(cursor_comp.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}

std::string& CoreUI::InputField::get_input_text() const {
	return text_comp->text;
}

void CoreUI::InputField::_update_cursor() {
	if (is_in_focus) {
		current_cursor_cd += TimeManager::deltaTime;
		if (current_cursor_cd >= cursor_blink_cd) {
			current_cursor_cd = 0.0f;
			cursor_visible = !cursor_visible;
			cursor_comp->is_visible = cursor_visible;
		}
	}
}

void CoreUI::InputField::_on_focus_get() {
	is_in_focus = true;
	cursor_comp->is_visible = true;

	std::string& field_text = text_comp->text;
	if (field_text.size() >= cursor_position)
		cursor_position = 0;
}

void CoreUI::InputField::_on_focus_lose() {
	is_in_focus = false;
	cursor_comp->is_visible = false;
	current_cursor_cd = 0.0f;
}

void CoreUI::InputField::_on_char_input(uint32_t code) {
	text_comp->add_char_to_text(char(code), cursor_position);
	cursor_position++;
	float cursor_offset = text_comp->get_char_offset(cursor_position);
	UI_ObjectManager::get_instance().get(cursor_comp.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}

void CoreUI::InputField::_on_char_remove() {
	if (cursor_position == 0) return;
	text_comp->remove_char_from_text(cursor_position - 1);
	cursor_position--;
	float cursor_offset = text_comp->get_char_offset(cursor_position);
	UI_ObjectManager::get_instance().get(cursor_comp.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}

void CoreUI::InputField::_on_cursor_position_changed(bool backwards) {
	std::string& field_text = text_comp->text;
	if (backwards && cursor_position > 0) cursor_position--;
	else if (!backwards && cursor_position < field_text.size()) cursor_position++;
	else return;
	cursor_visible = true;
	cursor_comp->is_visible = true;
	current_cursor_cd = 0.0f;
	float cursor_offset = text_comp->get_char_offset(cursor_position);
	UI_ObjectManager::get_instance().get(cursor_comp.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}