#include "UI/Canvas.h"
#include "UI/UI_ComponentManager.h"
#include "UI/UI_ObjectManager.h"

void CoreUI::Canvas::add_object(UI_Object* new_object) {
	objects.emplace_back(new_object->object_id);
}

void CoreUI::Canvas::update_canvas_objects_data(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	std::vector<UI_Text_Vertex2f>& sdf_text_buffer
) {
	for (auto& obj_id : objects) {
		UI_Object* obj = UI_ObjectManager::get_instance().get(obj_id);
		if(obj) update_child_object(*obj, render_queue, sprites_buffer, sdf_text_buffer);
	}
}

void CoreUI::Canvas::update_child_object(
	UI_Object& object,
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	std::vector<UI_Text_Vertex2f>& sdf_text_buffer
) {
	if (!object.is_enabled) return;

	UI_Component* comp = UI_ComponentManager::get_instance().get(object.object_id);
	if (comp && comp->is_visible) {
		if (comp->type == UI_Component_Type::UI_SPRITE) {
			comp->_update_sprite_buffer(render_queue, sprites_buffer);
		}
		else if (comp->type == UI_Component_Type::UI_TEXT) {
			comp->_update_sdf_text_buffer(render_queue, sdf_text_buffer);
		}
	}

	for (auto& child_id : object.transform.children) {
		UI_Object* child = UI_ObjectManager::get_instance().get(child_id);
		if (child) update_child_object(*child, render_queue, sprites_buffer, sdf_text_buffer);
	}
}