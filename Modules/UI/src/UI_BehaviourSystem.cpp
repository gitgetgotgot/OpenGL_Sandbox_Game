#include "UI/UI_BehaviourSystem.h"
#include "UI/UI_ComponentManager.h"
#include "UI/UI_ObjectManager.h"
#include "UI/InputField.h"
#include "UI/Button.h"
#include "UI/ScrollView.h"
#include <Utility/Math.h>
#include <IOSystem/SystemContext.h>
#include <Utility/TimeManager.h>

void CoreUI::UI_BehaviourSystem::update(UI_RenderContext& ctx) {
	bool raycast_is_captured = false;
	focus_changed_in_frame = false;
	prev_focused_id = focused_component_id;

	for (size_t i = ctx.hit_queue.size(); i-- > 0; ) {
		// take component
		UI_HitEntry& hit_entry = ctx.hit_queue[i];
		UI_ComponentBase* comp_base = static_cast<UI_ComponentBase*>(hit_entry.component);
		UI_Object* obj = UI_ObjectManager::get_instance().get(comp_base->object_id);

		// check if it is pointed by cursor (only if raycast is not blocked already)
		const glm::vec2& obj_offset = ctx.content_offsets[hit_entry.content_offset_id];
		bool pointed_now = !raycast_is_captured && GameMath::mouse_overlaps_ortho_square(
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
			process_SCROLL_VIEW_event(comp_base, pointed_now, obj->transform);
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

CoreUI::UI_BehaviourEntry* CoreUI::UI_BehaviourSystem::add(uint32_t ui_object_id, void* ptr, UI_Behaviour_VTable* vt) {
	return behaviours.add(UI_BehaviourEntry(ptr, vt), ui_object_id);
}

CoreUI::UI_BehaviourEntry* CoreUI::UI_BehaviourSystem::get(uint32_t ui_object_id) {
	return behaviours.get(ui_object_id);
}

bool CoreUI::UI_BehaviourSystem::remove(uint32_t ui_object_id) {
	return behaviours.remove(ui_object_id);
}

void CoreUI::UI_BehaviourSystem::process_BUTTON_event(UI_ComponentBase*& comp_base, bool& pointed_now) {
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

void CoreUI::UI_BehaviourSystem::process_INPUT_FIELD_event(UI_ComponentBase* comp_base, bool& pointed_now) {
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

void CoreUI::UI_BehaviourSystem::process_SCROLL_VIEW_event(UI_ComponentBase*& comp_base, bool& pointed_now, UI_Transform& tr) {
	auto scroll_view = static_cast<ScrollView*>(comp_base);

	//amount of children is changed -> replace all children
	if (scroll_view->last_children_size != tr.children.size()) {
		scroll_view->last_children_size = tr.children.size();
		//alignment is always Top Left
		const float initial_x_pos = tr.local_pos.x - tr.size.x * 0.5f + scroll_view->border_x_offset;
		const float initial_y_pos = tr.local_pos.y + tr.size.y * 0.5f - scroll_view->border_y_offset;
		
		float current_x_offset = initial_x_pos;
		float current_y_offset = initial_y_pos;

		float max_element_x_size = 0.0f;
		float max_element_y_size = 0.0f;

		const bool is_horizontal = scroll_view->layout == ScrollView::GroupLayout::LAYOUT_HORIZONTAL;
		const bool is_vertical   = scroll_view->layout == ScrollView::GroupLayout::LAYOUT_VERTICAL;

		for (auto& child_id : tr.children) {
			UI_Transform& child_tr = UI_ObjectManager::get_instance().get(child_id)->transform;
			child_tr.set_local_pos(
				current_x_offset + child_tr.size.x * 0.5f,
				current_y_offset - child_tr.size.y * 0.5f
			);
			if (is_horizontal) current_x_offset += child_tr.size.x + scroll_view->content_children_offset;
			if (is_vertical)   current_y_offset -= child_tr.size.y + scroll_view->content_children_offset;

			max_element_x_size = child_tr.size.x > max_element_x_size ? child_tr.size.x : max_element_x_size;
			max_element_y_size = child_tr.size.y > max_element_y_size ? child_tr.size.y : max_element_y_size;
		}
		current_x_offset += scroll_view->border_x_offset;
		current_y_offset -= scroll_view->border_y_offset;

		if (!is_horizontal) current_x_offset += max_element_x_size;
		if (!is_vertical) current_y_offset -= max_element_y_size;

		scroll_view->current_max_content_offset.x = tr.local_pos.x - tr.size.x * 0.5f - current_x_offset + tr.size.x;
		scroll_view->current_max_content_offset.y = tr.local_pos.y + tr.size.y * 0.5f - current_y_offset - tr.size.y;

		if (scroll_view->current_max_content_offset.x > 0.0f) scroll_view->current_max_content_offset.x = 0.0f;
		if (scroll_view->current_max_content_offset.y < 0.0f) scroll_view->current_max_content_offset.y = 0.0f;
	}

	if (pointed_now && SystemContext::mouse.wheel_offset != 0) {
		if (SystemContext::mouse.wheel_offset > 0.0f) {
			if (SystemContext::keyBoard.key_is_held(Key::KeyLeftShift)) {
				scroll_view->content_offset.x += SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view->content_offset.x > 0.0f) scroll_view->content_offset.x = 0.0f;
			}
			else {
				scroll_view->content_offset.y -= SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view->content_offset.y < 0.0f) scroll_view->content_offset.y = 0.0f;
			}
		}
		else {
			if (SystemContext::keyBoard.key_is_held(Key::KeyLeftShift)) {
				scroll_view->content_offset.x += SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view->content_offset.x < scroll_view->current_max_content_offset.x)
					scroll_view->content_offset.x = scroll_view->current_max_content_offset.x;
			}
			else {
				scroll_view->content_offset.y -= SystemContext::mouse.wheel_offset * scroll_view->mouse__wheel_scroll_speed;
				if (scroll_view->content_offset.y > scroll_view->current_max_content_offset.y)
					scroll_view->content_offset.y = scroll_view->current_max_content_offset.y;
			}
		}
	}
}