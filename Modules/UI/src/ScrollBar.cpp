#include "UI/ScrollBar.h"
#include "UI/UI_ObjectManager.h"
#include <IOSystem/SystemContext.h>
#include <algorithm>

void Engine::UI::ScrollBar::set_value(float value) {
	scroll_value = std::clamp(value, 0.0f, 1.0f);
	_update_thumb_pos();
}

void Engine::UI::ScrollBar::set_direction(ScrollDirection direction) {
	this->direction = direction;
}

void Engine::UI::ScrollBar::set_scroll_mode(ScrollMode mode) {
	scroll_mode = mode;
}

void Engine::UI::ScrollBar::set_steps_amount(uint16_t steps) {
	if (steps < 1 || steps > 1000) return;
	thumb_step_size = 1.0f / static_cast<float>(steps);
}

void Engine::UI::ScrollBar::set_thumb_size_ratio(float ratio) {
	// clamp so size is always between 5% and 100% of track_size
	const float thumb_size_ratio = std::clamp(ratio, 0.05f, 1.0f);
	
	UI_Object* track_obj = UI_ObjectManager::get_instance().get(track_img.get_id());
	UI_Object* thumb_obj = UI_ObjectManager::get_instance().get(thumb_img.get_id());
	if (!track_obj || !thumb_obj) return;

	if (direction == ScrollDirection::SCROLLBAR_HORIZONTAL) {
		const float thumb_size = track_obj->transform.size.x * thumb_size_ratio;
		thumb_obj->transform.set_width(thumb_size);
		min_thumb_offset = (thumb_size - track_obj->transform.size.x) * 0.5f;
		max_thumb_range = track_obj->transform.size.x - thumb_size;
	}
	else {
		const float thumb_size = track_obj->transform.size.y * thumb_size_ratio;
		thumb_obj->transform.set_height(thumb_size);
		min_thumb_offset = (track_obj->transform.size.y - thumb_size) * 0.5f;
		max_thumb_range = track_obj->transform.size.y - thumb_size;
	}

	_update_thumb_pos();
}

void Engine::UI::ScrollBar::bind_components(UI_Component_Ptr<Image>& track_img, UI_Component_Ptr<Image>& thumb_img) {
	this->track_img = track_img;
	this->thumb_img = thumb_img;

	UI_Object* track_obj = UI_ObjectManager::get_instance().get(track_img.get_id());
	UI_Object* thumb_obj = UI_ObjectManager::get_instance().get(thumb_img.get_id());
	if (!track_obj || !thumb_obj) return;

	if (direction == ScrollDirection::SCROLLBAR_HORIZONTAL) {
		min_thumb_offset = -track_obj->transform.size.x * 0.5f + thumb_obj->transform.size.x * 0.5f;
		max_thumb_range = track_obj->transform.size.x - thumb_obj->transform.size.x;
		scroll_raw_offset = min_thumb_offset;
	}
	else {
		min_thumb_offset = track_obj->transform.size.y * 0.5f - thumb_obj->transform.size.y * 0.5f;
		max_thumb_range = track_obj->transform.size.y - thumb_obj->transform.size.y;
		scroll_raw_offset = min_thumb_offset;
	}
}

void Engine::UI::ScrollBar::_update_thumb_pos() {
	UI_Object* thumb_obj = UI_ObjectManager::get_instance().get(thumb_img.get_id());
	if (!thumb_obj) return;

	if (direction == ScrollDirection::SCROLLBAR_HORIZONTAL) {
		scroll_raw_offset = min_thumb_offset + scroll_value * max_thumb_range;
		thumb_obj->transform.set_local_pos(scroll_raw_offset, 0.0f);
	}
	else {
		scroll_raw_offset = min_thumb_offset - scroll_value * max_thumb_range;
		thumb_obj->transform.set_local_pos(0.0f, scroll_raw_offset);
	}
}

void Engine::UI::ScrollBar::_on_track_held() {
	UI_Object* track_obj = UI_ObjectManager::get_instance().get(track_img.get_id());
	if (!track_obj || !max_thumb_range) return;

	float current_scroll_value = 0.0f;

	if (direction == ScrollDirection::SCROLLBAR_HORIZONTAL) {
		scroll_raw_offset = std::clamp(SystemContext::mouse.ortho_x_pos - track_obj->transform.global_pos.x, min_thumb_offset, min_thumb_offset + max_thumb_range);
		current_scroll_value = (scroll_raw_offset - min_thumb_offset) / max_thumb_range;
	}
	else {
		scroll_raw_offset = std::clamp(SystemContext::mouse.ortho_y_pos - track_obj->transform.global_pos.y, min_thumb_offset - max_thumb_range, min_thumb_offset);
		current_scroll_value = (min_thumb_offset - scroll_raw_offset) / max_thumb_range;
	}

	if (is_discrete) current_scroll_value = std::round(current_scroll_value / thumb_step_size) * thumb_step_size;
	if (current_scroll_value != scroll_value) {
		scroll_value = current_scroll_value;
		_update_thumb_pos();
		on_value_changed(scroll_value);
	}
}

void Engine::UI::ScrollBar::_on_thumb_drag(glm::vec2& scroll_thumb_cursor_delta) {
	UI_Object* track_obj = UI_ObjectManager::get_instance().get(track_img.get_id());
	if (!track_obj || !max_thumb_range) return;

	float current_scroll_value = 0.0f;

	if (direction == ScrollDirection::SCROLLBAR_HORIZONTAL) {
		scroll_raw_offset = std::clamp(
			SystemContext::mouse.ortho_x_pos - track_obj->transform.global_pos.x + scroll_thumb_cursor_delta.x, min_thumb_offset, min_thumb_offset + max_thumb_range
		);
		current_scroll_value = (scroll_raw_offset - min_thumb_offset) / max_thumb_range;
	}
	else {
		scroll_raw_offset = std::clamp(
			SystemContext::mouse.ortho_y_pos - track_obj->transform.global_pos.y + scroll_thumb_cursor_delta.y, min_thumb_offset - max_thumb_range, min_thumb_offset
		);
		current_scroll_value = (min_thumb_offset - scroll_raw_offset) / max_thumb_range;
	}

	if (is_discrete) current_scroll_value = std::round(current_scroll_value / thumb_step_size) * thumb_step_size;
	if (current_scroll_value != scroll_value) {
		scroll_value = current_scroll_value;
		_update_thumb_pos();
		on_value_changed(scroll_value);
	}
}

void Engine::UI::ScrollBar::_on_thumb_step(float thumb_global_x, float thumb_global_y) {
	if (direction == ScrollBar::SCROLLBAR_HORIZONTAL) {
		if (SystemContext::mouse.ortho_x_pos - thumb_global_x > 0.0f) scroll_raw_offset += max_thumb_range * 0.1f;
		else scroll_raw_offset -= max_thumb_range * 0.1f;
		scroll_raw_offset = std::clamp(scroll_raw_offset, min_thumb_offset, min_thumb_offset + max_thumb_range);
		scroll_value = (scroll_raw_offset - min_thumb_offset) / max_thumb_range;
	}
	else {
		if (SystemContext::mouse.ortho_y_pos - thumb_global_y > 0.0f) scroll_raw_offset += max_thumb_range * 0.1f;
		else scroll_raw_offset -= max_thumb_range * 0.1f;
		scroll_raw_offset = std::clamp(scroll_raw_offset, min_thumb_offset - max_thumb_range, min_thumb_offset);
		scroll_value = (min_thumb_offset - scroll_raw_offset) / max_thumb_range;
	}
	if (is_discrete)
		scroll_value = std::round(scroll_value / thumb_step_size) * thumb_step_size;
	_update_thumb_pos();
	on_value_changed(scroll_value);
}