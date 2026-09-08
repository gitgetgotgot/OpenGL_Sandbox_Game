#include "UI/UI_Component.h"
#include "UI/UI_ObjectManager.h"

void CoreUI::UI_Component::mark_dirty() {
	if (is_dirty) return;
	is_dirty = true;
	UI_ObjectManager::get_instance().mark_dirty_component(object_id);
}