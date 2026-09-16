#include "UI/Canvas.h"
#include "UI/UI_ComponentManager.h"
#include "UI/UI_ObjectManager.h"
#include "UI/CanvasManager.h"
#include "UI/UI_System.h"
#include <IOSystem/SystemContext.h>

void CoreUI::Canvas::add_object(UI_Obj_Ptr& object) {
	if (!object) return;
	objects.emplace_back(object.get_id());
}

void CoreUI::Canvas::update_canvas_objects_data(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
	std::vector<UI_Object*>& hit_queue
) {
	//every canvas always starts with default clip rectangle (id = 0), that can be changed in children
	for (auto& obj_id : objects) {
		UI_Object* obj = UI_ObjectManager::get_instance().get(obj_id);
		if(obj) update_child_object(*obj, render_queue, sprites_buffer, sdf_text_buffer, hit_queue, 0);
	}
}

void CoreUI::Canvas::update_child_object(
	UI_Object& object,
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
	std::vector<UI_Object*>& hit_queue,
	uint16_t clip_rect_id
) {
	if (!object.is_enabled) return;

	UI_ComponentEntry* comp = UI_ComponentManager::get_instance().get(object.object_id);
	if (comp) {
		UI_ComponentBase* base_comp = static_cast<UI_ComponentBase*>(comp->component);
		if (base_comp->type == UI_Component_Type::UI_PANEL) {
			clip_rect_id = add_clip_rect(object.transform);
		}
		if (base_comp->is_interactable) {
			hit_queue.push_back(&object);
		}
		if (base_comp->is_visible) {
			if (base_comp->render_type == UI_Render_Type::UI_SPRITE) {
				comp->vt->update_sprite_data(comp->component, render_queue, sprites_buffer, clip_rect_id);
			}
			else if (base_comp->render_type == UI_Render_Type::UI_TEXT) {
				comp->vt->update_text_data(comp->component, render_queue, sdf_text_buffer, clip_rect_id);
			}
		}
	}

	for (auto& child_id : object.transform.children) {
		UI_Object* child = UI_ObjectManager::get_instance().get(child_id);
		if (child) update_child_object(*child, render_queue, sprites_buffer, sdf_text_buffer, hit_queue, clip_rect_id);
	}
}

uint16_t CoreUI::Canvas::add_clip_rect(UI_Transform& tr) {
	uint32_t x = ((tr.global_pos.x - tr.size.x * 0.5f) * SystemContext::screen.double_x_ratio + 0.5f) * SystemContext::screen.width;
	uint32_t y = ((tr.global_pos.y - tr.size.y * 0.5f) * 0.5f + 0.5f) * SystemContext::screen.height;
	uint32_t w = tr.size.x * SystemContext::screen.double_x_ratio * SystemContext::screen.width;
	uint32_t h = tr.size.y * 0.5f * SystemContext::screen.height;
	return UI_System::get_instance().add_clip_rect(x, y, w, h);
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