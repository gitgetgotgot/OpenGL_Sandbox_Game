#include "UI/InputField.h"
#include "UI/UI_ObjectManager.h"
#include <Utility/TimeManager.h>

void Engine::UI::InputField::bind_components(
	UI_Component_Ptr<Image>& field,
	UI_Component_Ptr<SDF_Text>& text,
	UI_Component_Ptr<Image>& cursor
) {
	this->field = field;
	field_text = text;
	field_cursor = cursor;
	field_cursor->is_visible = false;
	float cursor_offset = field_text->get_char_offset(cursor_position);
	cursor_initial_x_pos = -UI_ObjectManager::get_instance().get(field_text.get_id())->transform.size.x * 0.5f;
	auto& cursor_tr = UI_ObjectManager::get_instance().get(field_cursor.get_id())->transform;
	cursor_initial_x_pos += cursor_tr.size.x * 0.5f;
	cursor_tr.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}

std::string& Engine::UI::InputField::get_input_text() const {
	return field_text->text;
}

bool Engine::UI::InputField::in_focus() const {
	return is_in_focus;
}

void Engine::UI::InputField::_update_cursor() {
	if (is_in_focus) {
		current_cursor_cd += TimeManager::deltaTime;
		if (current_cursor_cd >= cursor_blink_cd) {
			current_cursor_cd = 0.0f;
			cursor_visible = !cursor_visible;
			field_cursor->is_visible = cursor_visible;
		}
	}
}

void Engine::UI::InputField::_on_focus_get() {
	is_in_focus = true;
	field_cursor->is_visible = true;

	std::string& f_text = field_text->text;
	if (f_text.size() < cursor_position) {
		cursor_position = 0;
		float cursor_offset = field_text->get_char_offset(cursor_position);
		UI_ObjectManager::get_instance().get(field_cursor.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
	}
}

void Engine::UI::InputField::_on_focus_lose() {
	is_in_focus = false;
	field_cursor->is_visible = false;
	current_cursor_cd = 0.0f;
}

void Engine::UI::InputField::_on_char_input(uint32_t code) {
	field_text->add_char_to_text(char(code), cursor_position);
	cursor_position++;
	float cursor_offset = field_text->get_char_offset(cursor_position);
	UI_ObjectManager::get_instance().get(field_cursor.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}

void Engine::UI::InputField::_on_char_remove() {
	if (cursor_position == 0) return;
	field_text->remove_char_from_text(cursor_position - 1);
	cursor_position--;
	float cursor_offset = field_text->get_char_offset(cursor_position);
	UI_ObjectManager::get_instance().get(field_cursor.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}

void Engine::UI::InputField::_on_cursor_position_changed(bool backwards) {
	std::string& f_text = field_text->text;
	if (backwards && cursor_position > 0) cursor_position--;
	else if (!backwards && cursor_position < f_text.size()) cursor_position++;
	else return;
	cursor_visible = true;
	field_cursor->is_visible = true;
	current_cursor_cd = 0.0f;
	float cursor_offset = field_text->get_char_offset(cursor_position);
	UI_ObjectManager::get_instance().get(field_cursor.get_id())->transform.set_local_pos(cursor_initial_x_pos + cursor_offset, 0.0f);
}