#include "UI/UI_BehaviourSystem.h"
#include "UI/UI_CompManager.h"
#include "UI/UI_ObjectManager.h"
#include "UI/InputField.h"
#include "UI/Button.h"
#include "UI/ScrollView.h"
#include "UI/ScrollBar.h"
#include <Utility/Math.h>
#include <IOSystem/SystemContext.h>
#include <Utility/TimeManager.h>
#include <algorithm>

void Engine::UI::UI_BehaviourSystem::update(UI_RenderContext& ctx) {
	bool raycast_is_captured = false;
	focus_changed_in_frame = false;
	prev_focused_id = focused_component_id;

	for (size_t i = ctx.hit_queue.size(); i-- > 0; ) {
		// take component
		UI_HitEntry& hit_entry = ctx.hit_queue[i];
		UI_ComponentBase* comp_base = static_cast<UI_ComponentBase*>(hit_entry.component);
		UI_Object* obj = UI_ObjectManager::get_instance().get(comp_base->object_id);

		// check if it is pointed by cursor (only if raycast is not blocked already and cursor is in clip rect)
		const glm::vec2& obj_offset = ctx.content_offsets[hit_entry.content_offset_id];
		const bool cursor_in_clip_rect = ctx.clip_rects[hit_entry.clip_rect_id].mouse_inside;
		bool pointed_now = !raycast_is_captured && cursor_in_clip_rect && GameMath::mouse_overlaps_ortho_square(
			obj->transform.global_pos, obj->transform.size,
			SystemContext::mouse.ortho_x_pos - obj_offset.x, SystemContext::mouse.ortho_y_pos - obj_offset.y);

		// process system behaviour for specific UI components
		switch (comp_base->type) {
		case UI_Component_Type::UI_BUTTON:
			process_BUTTON_event(comp_base, pointed_now);
			break;
		case UI_Component_Type::UI_INPUT_FIELD:
			process_INPUT_FIELD_event(comp_base, pointed_now);
			break;
		case UI_Component_Type::UI_SCROLL_VIEW:
			process_SCROLL_VIEW_event(comp_base, pointed_now);
			break;
		case UI_Component_Type::UI_SCROLL_BAR:
			process_SCROLL_BAR_event(comp_base, pointed_now);
			break;
		}

		// process custom behaviour for components, if such is available
		UI_BehaviourEntry* b_entry = behaviours.get(obj->object_id);
		if (b_entry) {
			UI_BehaviourBase* base = static_cast<UI_BehaviourBase*>(b_entry->behaviour);

			if (pointed_now) {
				if (!base->is_pointed) {
					base->is_pointed = true;
					if (b_entry->vt->on_pointer_enter)
						b_entry->vt->on_pointer_enter(b_entry->behaviour);
				}
				if (b_entry->vt->on_pointer_hover)
					b_entry->vt->on_pointer_hover(b_entry->behaviour);

				if (SystemContext::mouse.lb_is_pressed()) {
					base->is_pressed = true;
					if (b_entry->vt->on_pointer_down)
						b_entry->vt->on_pointer_down(b_entry->behaviour);
				}
			}
			else if (base->is_pointed) {
				base->is_pointed = false;
				if (b_entry->vt->on_pointer_exit)
					b_entry->vt->on_pointer_exit(b_entry->behaviour);
			}

			if (base->is_pressed) {
				if (SystemContext::mouse.lb_is_held()) {
					if (b_entry->vt->on_pointer_held)
						b_entry->vt->on_pointer_held(b_entry->behaviour);
				}
				if (SystemContext::mouse.lb_is_released()) {
					base->is_pressed = false;
					if (b_entry->vt->on_pointer_up)
						b_entry->vt->on_pointer_up(b_entry->behaviour);
				}
			}
		}

		// block raycast
		if (pointed_now && comp_base->blocks_input)
			raycast_is_captured = true;

	}

	if (SystemContext::mouse.lb_is_pressed() && !focus_changed_in_frame)
		focused_component_id = 0;
}

Engine::UI::UI_BehaviourEntry* Engine::UI::UI_BehaviourSystem::add(uint32_t ui_object_id, void* ptr, UI_Behaviour_VTable* vt) {
	return behaviours.add(UI_BehaviourEntry(ptr, vt), ui_object_id);
}

Engine::UI::UI_BehaviourEntry* Engine::UI::UI_BehaviourSystem::get(uint32_t ui_object_id) {
	return behaviours.get(ui_object_id);
}

bool Engine::UI::UI_BehaviourSystem::remove(uint32_t ui_object_id) {
	return behaviours.remove(ui_object_id);
}

void Engine::UI::UI_BehaviourSystem::process_BUTTON_event(UI_ComponentBase*& comp_base, bool& pointed_now) {
	auto btn = static_cast<Button*>(comp_base);
	// on hover
	if (pointed_now && !btn->_is_pointed) {
		btn->_is_pointed = true;
		if(btn->_is_pressed)
			btn->_on_cursor_pressed();
		else
			btn->_on_cursor_enter();
	}
	// on exit
	if (!pointed_now && btn->_is_pointed) {
		btn->_is_pointed = false;
		btn->_on_cursor_exit();
	}
	// on press
	if (pointed_now && SystemContext::mouse.lb_is_pressed()) {
		btn->_is_pressed = true;
		btn->_on_cursor_pressed();
	}
	// on release
	if (SystemContext::mouse.lb_is_released() && btn->_is_pressed) {
		btn->_is_pressed = false;
		if (pointed_now) btn->_on_callback_use();
	}
}

void Engine::UI::UI_BehaviourSystem::process_INPUT_FIELD_event(UI_ComponentBase* comp_base, bool& pointed_now) {
	auto field = static_cast<InputField*>(comp_base);
	// lose focus
	if (SystemContext::mouse.lb_is_pressed() && !pointed_now && prev_focused_id == comp_base->object_id) {
		field->_on_focus_lose();
		return;
	}
	// get focus
	else if (SystemContext::mouse.lb_is_pressed() && pointed_now && focused_component_id != comp_base->object_id) {
		field->_on_focus_get();
		focused_component_id = comp_base->object_id;
		focus_changed_in_frame = true;
		return;
	}
	
	field->_update_cursor();

	// process input character if has focus
	if (focused_component_id == comp_base->object_id) {
		for (uint32_t& code : SystemContext::keyBoard.currentPressedChars) {
			field->_on_char_input(code);
		}

		if (SystemContext::keyBoard.key_is_pressed(Key::KeyLeft)) {
			field->_on_cursor_position_changed(true);
		}
		else if (SystemContext::keyBoard.key_is_pressed(Key::KeyRight)) {
			field->_on_cursor_position_changed(false);
		}

		if (SystemContext::keyBoard.key_is_pressed(Key::KeyBackspace)) {
			field->_on_char_remove();
		}
	}
}

void Engine::UI::UI_BehaviourSystem::process_SCROLL_VIEW_event(UI_ComponentBase*& comp_base, bool& pointed_now) {
	if (pointed_now && SystemContext::mouse.wheel_offset != 0) {

		auto scroll_view = static_cast<ScrollView*>(comp_base);
		auto& scroll_view_content = *scroll_view->content;

		if (SystemContext::mouse.wheel_offset > 0.0f) {
			if (SystemContext::keyBoard.key_is_held(Key::KeyLeftShift)) {
				scroll_view_content.content_offset.x += SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view_content.content_offset.x > 0.0f) scroll_view_content.content_offset.x = 0.0f;
			}
			else {
				scroll_view_content.content_offset.y -= SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view_content.content_offset.y < 0.0f) scroll_view_content.content_offset.y = 0.0f;
			}
		}
		else {
			if (SystemContext::keyBoard.key_is_held(Key::KeyLeftShift)) {
				scroll_view_content.content_offset.x += SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view_content.content_offset.x < scroll_view_content.current_max_content_offset.x)
					scroll_view_content.content_offset.x = scroll_view_content.current_max_content_offset.x;
			}
			else {
				scroll_view_content.content_offset.y -= SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view_content.content_offset.y > scroll_view_content.current_max_content_offset.y)
					scroll_view_content.content_offset.y = scroll_view_content.current_max_content_offset.y;
			}
		}
		if (scroll_view->has_vertical_scrollbar && scroll_view->scrollbar_Y && scroll_view_content.current_max_content_offset.y) {
			scroll_view->scrollbar_Y->set_value(scroll_view_content.content_offset.y / scroll_view_content.current_max_content_offset.y);
		}
	}
}

void Engine::UI::UI_BehaviourSystem::process_SCROLL_BAR_event(UI_ComponentBase*& comp_base, bool& pointed_now) {
	auto scroll_bar = static_cast<ScrollBar*>(comp_base);

	if (scroll_bar->scroll_mode == ScrollBar::ScrollMode::SLIDER_MODE) {
		if (pointed_now && SystemContext::mouse.lb_is_pressed()) {
			//scroll_bar->_on_track_held();
			scroll_bar->_is_dragged = true;
		}
		if (scroll_bar->_is_dragged) {
			if (SystemContext::mouse.lb_is_released()) {
				scroll_bar->_is_dragged = false;
			}
			else {
				scroll_bar->_on_track_held();
			}
		}
	}
	else { // PAGEABLE_MODE
		UI_Object* thumb_obj = UI_ObjectManager::get_instance().get(scroll_bar->thumb_img.get_id());
		if (!thumb_obj) return;

		UI_Transform& thumb_tr = thumb_obj->transform;
		const bool cursor_over_thumb = GameMath::mouse_overlaps_ortho_square(
			thumb_tr.global_pos, thumb_tr.size, SystemContext::mouse.ortho_x_pos, SystemContext::mouse.ortho_y_pos
		);
		if (scroll_bar->_is_dragged) {
			scroll_bar->_on_thumb_drag(scroll_thumb_cursor_delta);
		}
		if (scroll_bar->_is_dragged && SystemContext::mouse.lb_is_released()) {
			scroll_bar->_is_dragged = false;
		}
		else if (cursor_over_thumb && SystemContext::mouse.lb_is_pressed()) {
			scroll_bar->_is_dragged = true;
			scroll_thumb_cursor_delta.x = thumb_tr.global_pos.x - SystemContext::mouse.ortho_x_pos;
			scroll_thumb_cursor_delta.y = thumb_tr.global_pos.y - SystemContext::mouse.ortho_y_pos;
		}
		else if (pointed_now && SystemContext::mouse.lb_is_pressed()) {
			scroll_bar->_on_thumb_step(thumb_tr.global_pos.x, thumb_tr.global_pos.y);
		}
	}
}