#include "UI/UI_ComponentManager.h"

void CoreUI::UI_ComponentManager::ClearData() {
	components = {};
}

CoreUI::UI_Component* CoreUI::UI_ComponentManager::add(std::unique_ptr<UI_Component> comp) {
	return components.add(std::move(comp), comp->object_id)->get();
}

bool CoreUI::UI_ComponentManager::remove(uint32_t ui_object_id) {
	return components.remove(ui_object_id);
}

CoreUI::UI_Component* CoreUI::UI_ComponentManager::get(uint32_t ui_object_id) {
	return components.get(ui_object_id)->get();
}