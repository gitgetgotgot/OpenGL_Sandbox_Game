#include "UI/UI_BehaviourSystem.h"
#include "UI/UI_ComponentManager.h"
#include <Utility/Math.h>
#include <IOSystem/SystemContext.h>

void CoreUI::UI_BehaviourSystem::update() {
	bool raycast_is_captured = false;

	for (size_t i = hit_queue.size(); i-- > 0; ) {
		UI_Object* obj = hit_queue[i];
		UI_Component* comp = UI_ComponentManager::get_instance().get(obj->object_id);

		bool pointed_now = !raycast_is_captured && GameMath::mouse_overlaps_ortho_square(
			obj->transform.global_pos, obj->transform.size,
			SystemContext::mouse.ortho_x_pos, SystemContext::mouse.ortho_y_pos);

		UI_BehaviourEntry* b_entry = behaviours.get(comp->object_id);
		UI_BehaviourBase* base = b_entry->Base();

		if (pointed_now) {
			if (!base->is_pointed) {
				base->is_pointed = true;
				if (b_entry->vt->on_pointer_enter)
					b_entry->vt->on_pointer_enter(b_entry->obj);
			}
			if (b_entry->vt->on_pointer_hover)
				b_entry->vt->on_pointer_hover(b_entry->obj);

			if (SystemContext::mouse.lb_is_pressed()) {
				base->is_pressed = true;
				if (b_entry->vt->on_pointer_down)
					b_entry->vt->on_pointer_down(b_entry->obj);
			}

			if (comp->blocks_input) raycast_is_captured = true;
		}
		else if (base->is_pointed) {
			base->is_pointed = false;
			if (b_entry->vt->on_pointer_exit)
				b_entry->vt->on_pointer_exit(b_entry->obj);
		}

		if (base->is_pressed) {
			if (SystemContext::mouse.lb_is_held() && base->is_pressed) {
				if (b_entry->vt->on_pointer_held)
					b_entry->vt->on_pointer_held(b_entry->obj);
			}
			if (SystemContext::mouse.lb_is_released() && base->is_pressed) {
				base->is_pressed = false;
				if (b_entry->vt->on_pointer_up)
					b_entry->vt->on_pointer_up(b_entry->obj);
			}
		}
	}
}

CoreUI::UI_BehaviourEntry& CoreUI::UI_BehaviourSystem::add(uint32_t ui_object_id, void* ptr, UI_Behaviour_VTable* vt) {
	UI_BehaviourEntry* entry = behaviours.add(UI_BehaviourEntry{}, ui_object_id);
	entry->obj = ptr;
	entry->vt = vt;
	return *entry;
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