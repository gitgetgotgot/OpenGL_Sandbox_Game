#include "UI/UI_BehaviourSystem.h"
#include "UI/UI_ComponentManager.h"
#include "UI/InputField.h"
#include "UI/Button.h"
#include <Utility/Math.h>
#include <IOSystem/SystemContext.h>

void CoreUI::UI_BehaviourSystem::update() {
	bool raycast_is_captured = false;
	focus_changed_in_frame = false;
	prev_focused_id = focused_component_id;

	for (size_t i = hit_queue.size(); i-- > 0; ) {
		// take component
		UI_Object* obj = hit_queue[i];
		UI_ComponentBase* comp_base = static_cast<UI_ComponentBase*>
			(UI_ComponentManager::get_instance().get(obj->object_id)->component);

		// check if it is pointed by cursor (only if raycast is not blocked already)
		bool pointed_now = !raycast_is_captured && GameMath::mouse_overlaps_ortho_square(
			obj->transform.global_pos, obj->transform.size,
			SystemContext::mouse.ortho_x_pos, SystemContext::mouse.ortho_y_pos);

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

std::vector<CoreUI::UI_Object*>& CoreUI::UI_BehaviourSystem::get_hit_queue() {
	return hit_queue;
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
	// lose focus
	if (SystemContext::mouse.lb_is_pressed() && !pointed_now && prev_focused_id == comp_base->object_id) {
		static_cast<InputField*>(comp_base)->_on_focus_lose();
		return;
	}
	// get focus
	else if (SystemContext::mouse.lb_is_pressed() && pointed_now && focused_component_id != comp_base->object_id) {
		static_cast<InputField*>(comp_base)->_on_focus_get();
		focused_component_id = comp_base->object_id;
		focus_changed_in_frame = true;
		return;
	}
	
	static_cast<InputField*>(comp_base)->_update_cursor();

	// process input character if has focus
	if (focused_component_id == comp_base->object_id) {
		for (uint32_t& code : SystemContext::keyBoard.currentPressedChars) {
			static_cast<InputField*>(comp_base)->_on_char_input(code);
		}

		if (SystemContext::keyBoard.key_is_pressed(Key::KeyLeft)) {
			static_cast<InputField*>(comp_base)->_on_cursor_position_changed(true);
		}
		else if (SystemContext::keyBoard.key_is_pressed(Key::KeyRight)) {
			static_cast<InputField*>(comp_base)->_on_cursor_position_changed(false);
		}

		if (SystemContext::keyBoard.key_is_pressed(Key::KeyBackspace)) {
			static_cast<InputField*>(comp_base)->_on_char_remove();
		}
	}
}

void CoreUI::UI_BehaviourSystem::process_SCROLL_VIEW_event(UI_ComponentBase*& comp_base, bool& pointed_now) {

}