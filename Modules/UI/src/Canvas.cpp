#include "UI/Canvas.h"
#include "UI/UI_ComponentManager.h"
#include "UI/UI_ObjectManager.h"
#include "UI/CanvasManager.h"
#include "UI/UI_System.h"
#include "UI/ScrollView.h"
#include <IOSystem/SystemContext.h>

void CoreUI::Canvas::add_object(UI_Obj_Ptr& object) {
	if (!object) return;
	objects.emplace_back(object.get_id());
}

void CoreUI::Canvas::update_canvas_objects_data(UI_RenderContext& ctx, UI_RenderState state) {
	//every canvas always starts with default clip rectangle (id = 0), that can be changed in children
	for (auto& obj_id : objects) {
		UI_Object* obj = UI_ObjectManager::get_instance().get(obj_id);
		if(obj && obj->is_enabled) update_child_object(*obj, ctx, state);
	}
}

void CoreUI::Canvas::update_child_object(UI_Object& object, UI_RenderContext& ctx, UI_RenderState state) {
	UI_ComponentEntry* comp = UI_ComponentManager::get_instance().get(object.object_id);
	if (comp) {
		UI_ComponentBase* base_comp = static_cast<UI_ComponentBase*>(comp->component);
		const bool is_renderable = base_comp->render_type != UI_Render_Type::UI_NONE;
		const bool is_interactable = base_comp->is_interactable;
		bool is_inside_clip_rect_area = false;

		if (is_renderable || is_interactable) {
			const glm::vec2& pos = object.transform.global_pos + ctx.content_offsets[state.content_offset_id];
			if (ctx.clip_rects[state.clip_rect_id].overlaps_with_box(pos, object.transform.size)) is_inside_clip_rect_area = true;
		}
		if (is_inside_clip_rect_area) {
			if (is_interactable) {
				if (ctx.clip_rects[state.clip_rect_id].overlaps_with_mouse(
					SystemContext::mouse.ortho_x_pos, SystemContext::mouse.ortho_y_pos
				)) ctx.hit_queue.emplace_back(comp->component, state.content_offset_id);
			}
			if (is_renderable && base_comp->is_visible) {
				comp->vt->update_buffer_data(comp->component, ctx, state);
			}
			if (base_comp->type == UI_Component_Type::UI_PANEL) {
				state.clip_rect_id = add_clip_rect(object.transform, ctx.clip_rects);
			}
			else if (base_comp->type == UI_Component_Type::UI_SCROLL_VIEW) {
				state.content_offset_id = add_content_offset(static_cast<ScrollView*>(base_comp)->_get_content_offset(), ctx.content_offsets);
			}
		}
		else {
			return; //just stop if parent is not inside a clip rect
		}
	}
	for (auto& child_id : object.transform.children) {
		UI_Object* child = UI_ObjectManager::get_instance().get(child_id);
		if (child) update_child_object(*child, ctx, state);
	}
}

uint16_t CoreUI::Canvas::add_clip_rect(const UI_Transform& tr, std::vector<ClipRectangle>& clip_rects) {
	uint32_t x = ((tr.global_pos.x - tr.size.x * 0.5f) * SystemContext::screen.double_x_ratio + 0.5f) * SystemContext::screen.width;
	uint32_t y = ((tr.global_pos.y - tr.size.y * 0.5f) * 0.5f + 0.5f) * SystemContext::screen.height;
	uint32_t w = tr.size.x * SystemContext::screen.double_x_ratio * SystemContext::screen.width;
	uint32_t h = tr.size.y * 0.5f * SystemContext::screen.height;
	clip_rects.emplace_back(x, y, w, h, tr.global_pos.x, tr.global_pos.y, tr.size.x, tr.size.y);
	return clip_rects.size() - 1;
}

uint16_t CoreUI::Canvas::add_content_offset(const glm::vec2& offset, std::vector<glm::vec2>& content_offsets) {
	content_offsets.emplace_back(offset.x, offset.y);
	return content_offsets.size() - 1;
}


CoreUI::Canvas* CoreUI::UI_Canvas_Ptr::operator->() const {
	return CanvasManager::get_instance().get(id);
}

CoreUI::Canvas& CoreUI::UI_Canvas_Ptr::operator*() const {
	return *CanvasManager::get_instance().get(id);
}

CoreUI::UI_Canvas_Ptr::operator bool() const {
	return id != 0 && CanvasManager::get_instance().get(id);
}

CoreUI::UI_Canvas_Ptr& CoreUI::UI_Canvas_Ptr::operator=(const UI_Canvas_Ptr& other) noexcept {
	if (this != &other) {
		this->id = other.id;
	}
	return *this;
}

CoreUI::UI_Canvas_Ptr& CoreUI::UI_Canvas_Ptr::operator=(UI_Canvas_Ptr&& other) noexcept {
	if (this != &other) {
		this->id = other.id;
		other.id = 0;
	}
	return *this;
}