#include "UI/UI_ObjectManager.h"
#include "UI/UI_ComponentManager.h"

void CoreUI::UI_ObjectManager::ClearData() {
	objects = {};
	free_ids = {};
	current_max_id = 1;
}

CoreUI::UI_Obj_Ptr CoreUI::UI_ObjectManager::add() {
	uint32_t id;
	if (!free_ids.empty()) {
		id = free_ids.back();
		free_ids.pop_back();
	}
	else id = current_max_id++;
	objects.add(UI_Object(id), id);

	return UI_Obj_Ptr(id);
}

bool CoreUI::UI_ObjectManager::remove(uint32_t ui_object_id) {
	bool deleted = objects.remove(ui_object_id);
	if (deleted)
		free_ids.emplace_back(ui_object_id);
	return deleted;
}

CoreUI::UI_Object* CoreUI::UI_ObjectManager::get(uint32_t ui_object_id) {
	return objects.get(ui_object_id);
}

void CoreUI::UI_ObjectManager::mark_dirty_transform(uint32_t ui_object_id) {
	dirty_objects[ui_object_id].dirty_transform = true;
}

void CoreUI::UI_ObjectManager::mark_dirty_component(uint32_t ui_object_id) {
	dirty_objects[ui_object_id].dirty_component = true;
}

void CoreUI::UI_ObjectManager::update_dirty_objects() {
	if (dirty_objects.empty()) return;
	
	for (auto& it : dirty_objects) {
		if (it.second.dirty_transform) {
			//update transform and component
			get(it.first)->transform.update_transform();
			UI_ComponentEntry* entry = UI_ComponentManager::get_instance().get(it.first);
			if(static_cast<UI_ComponentBase*>(entry->component)->render_type != UI_Render_Type::UI_NONE)
				entry->vt->update_render_data(entry->component);
		}
		else if (it.second.dirty_component) {
			//update only component
			UI_ComponentEntry* entry = UI_ComponentManager::get_instance().get(it.first);
			entry->vt->update_render_data(entry->component);
		}
	}
	dirty_objects.clear();
}