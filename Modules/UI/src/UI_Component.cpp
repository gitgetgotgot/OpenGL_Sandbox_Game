#include "UI/UI_Component.h"
#include "UI/UI_ObjectManager.h"

void CoreUI::UI_ComponentBase::update_render_queue(std::vector<UI_RenderEntry>& render_queue, uint16_t clip_rect_id, uint32_t index_count) {
	UI_RenderEntry& last_entry = render_queue.back();
	if (clip_rect_id == last_entry.clip_rect_id) {
		if (last_entry.render_type == render_type) {
			last_entry.index_count += index_count;
		}
		else {
			render_queue.emplace_back(render_type, index_count, clip_rect_id, false);
		}
	}
	else {
		render_queue.emplace_back(render_type, index_count, clip_rect_id, true);
	}
}

void CoreUI::UI_ComponentBase::mark_dirty() {
	if (is_dirty) return;
	is_dirty = true;
	UI_ObjectManager::get_instance().mark_dirty_component(object_id);
}