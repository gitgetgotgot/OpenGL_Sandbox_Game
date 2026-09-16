#include "UI/UI_ComponentManager.h"

void CoreUI::UI_ComponentManager::ClearData() {
	components = {};
}

CoreUI::UI_ComponentEntry* CoreUI::UI_ComponentManager::add(void* comp, UI_Component_VTable* comp_vt, uint32_t ui_object_id) {
	return components.add(UI_ComponentEntry(comp, comp_vt), ui_object_id);
}

bool CoreUI::UI_ComponentManager::remove(uint32_t ui_object_id) {
	return components.remove(ui_object_id);
}

CoreUI::UI_ComponentEntry* CoreUI::UI_ComponentManager::get(uint32_t ui_object_id) {
	return components.get(ui_object_id);
}

uint32_t CoreUI::UI_ComponentManager::size() {
	return components.size();
}