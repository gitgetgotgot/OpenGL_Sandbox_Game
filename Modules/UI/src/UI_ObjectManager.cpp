#include "UI/UI_ObjectManager.h"
#include "UI/UI_CompManager.h"

void Engine::UI::UI_ObjectManager::ClearData() {
	objects = {};
	free_ids = {};
	current_max_id = 1;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_ObjectManager::add() {
	uint32_t id;
	if (!free_ids.empty()) {
		id = free_ids.back();
		free_ids.pop_back();
	}
	else id = current_max_id++;
	objects.add(UI_Object(id), id);

	return UI_Obj_Ptr(id);
}

void Engine::UI::UI_ObjectManager::destroy(uint32_t ui_object_id) {
	auto obj_ptr = get(ui_object_id);
	if (obj_ptr) {
		obj_ptr->transform.mark_destroyed(); // mark transform and all its children as destroyed
	}
}

void Engine::UI::UI_ObjectManager::mark_destroyed_object(uint32_t ui_object_id) {
	destroyed_objects.push_back(ui_object_id);
}

Engine::UI::UI_Object* Engine::UI::UI_ObjectManager::get(uint32_t ui_object_id) {
	return objects.get(ui_object_id);
}

void Engine::UI::UI_ObjectManager::mark_dirty_transform(uint32_t ui_object_id) {
	dirty_objects[ui_object_id].dirty_transform = true;
}

void Engine::UI::UI_ObjectManager::mark_dirty_component(uint32_t ui_object_id) {
	dirty_objects[ui_object_id].dirty_component = true;
}

void Engine::UI::UI_ObjectManager::unmark_dirty_transform(uint32_t ui_object_id) {
	dirty_objects[ui_object_id].dirty_transform = false;
}

void Engine::UI::UI_ObjectManager::unmark_dirty_component(uint32_t ui_object_id) {
	dirty_objects[ui_object_id].dirty_component = false;
}

void Engine::UI::UI_ObjectManager::update_dirty_objects() {
	if (dirty_objects.empty()) return;
	
	for (auto& it : dirty_objects) {
		if (it.second.dirty_transform) {
			//update transform and component
			get(it.first)->transform.update_transform();
			UI_ComponentEntry* entry = UI_ComponentManager::Instance().get(it.first);
			if(entry && static_cast<UI_ComponentBase*>(entry->component)->render_type != UI_Render_Type::UI_NONE)
				entry->vt->update_data(entry->component);
		}
		else if (it.second.dirty_component) {
			//update only component
			UI_ComponentEntry* entry = UI_ComponentManager::Instance().get(it.first);
			entry->vt->update_data(entry->component);
		}
	}
	dirty_objects.clear();
}

void Engine::UI::UI_ObjectManager::remove_destroyed_objects() {
	if (destroyed_objects.empty()) return;

	for (const auto& obj_id : destroyed_objects) {
		// component and object are safely removed if they exist
		UI_ComponentManager::Instance().remove(obj_id);
		objects.remove(obj_id);
		// check if parent is not marked as destroyed and remove this obj from its children
	}
	destroyed_objects.clear();
}