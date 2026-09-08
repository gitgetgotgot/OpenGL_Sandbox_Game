#include "UI/UI_ObjectManager.h"
#include "UI/UI_ComponentManager.h"

void CoreUI::UI_ObjectManager::ClearData() {
	objects = {};
	free_ids = {};
	current_max_id = 1;
}

CoreUI::UI_Object* CoreUI::UI_ObjectManager::add() {
	uint32_t id;
	if (!free_ids.empty()) {
		id = free_ids.back();
		free_ids.pop_back();
	}
	else id = current_max_id++;

	return objects.add(UI_Object(id), id);
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
#include <iostream>
void CoreUI::UI_ObjectManager::update_dirty_objects() {
	if (dirty_objects.empty()) return;
	std::cout << "Dirty UI objects were updated\n";
	for (auto& it : dirty_objects) {
		if (it.second.dirty_transform) {
			//update transform and component
			get(it.first)->transform.update_transform();
			UI_ComponentManager::get_instance().get(it.first)->_update_render_data();
		}
		else if (it.second.dirty_component) {
			//update only component
			UI_ComponentManager::get_instance().get(it.first)->_update_render_data();
		}
	}
	dirty_objects.clear();
}