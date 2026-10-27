#include "UI/UI_CompManager.h"

void Engine::UI::UI_ComponentManager::ClearData() {
	components = {};
}

Engine::UI::UI_ComponentEntry* Engine::UI::UI_ComponentManager::add(void* comp, UI_Component_VTable* comp_vt, uint32_t ui_object_id) {
	return components.add(UI_ComponentEntry(comp, comp_vt), ui_object_id);
}

bool Engine::UI::UI_ComponentManager::remove(uint32_t ui_object_id) {
	return components.remove(ui_object_id);
}

Engine::UI::UI_ComponentEntry* Engine::UI::UI_ComponentManager::get(uint32_t ui_object_id) {
	return components.get(ui_object_id);
}

uint32_t Engine::UI::UI_ComponentManager::size() {
	return components.size();
}