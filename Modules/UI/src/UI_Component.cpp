#include "UI/UI_Component.h"
#include "UI/UI_ObjectManager.h"

void CoreUI::UI_ComponentBase::update_render_queue(UI_RenderContext& c, UI_RenderState& s, uint32_t index_count) {
	UI_RenderEntry& last_entry = c.render_queue.back();
	const bool same_rect = s.clip_rect_id == last_entry.clip_rect_id;
	const bool same_content_offset = s.content_offset_id == last_entry.content_offset_id;

	if (same_rect && same_content_offset && render_type == last_entry.render_type)
		last_entry.index_count += index_count;	
	else
		c.render_queue.emplace_back(render_type, index_count, s.clip_rect_id, s.content_offset_id, !same_rect, !same_content_offset);
}

void CoreUI::UI_ComponentBase::mark_dirty() {
	if (is_dirty) return;
	is_dirty = true;
	UI_ObjectManager::get_instance().mark_dirty_component(object_id);
}