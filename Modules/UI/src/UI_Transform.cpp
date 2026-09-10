#include "UI/UI_Transform.h"
#include "UI/UI_ObjectManager.h"

void CoreUI::UI_Transform::set_local_pos(float x, float y) {
	local_pos.x = x; local_pos.y = y;
	if (!is_dirty) mark_dirty();
}

void CoreUI::UI_Transform::set_local_pos(glm::vec2 pos) {
	local_pos = pos;
	if (!is_dirty) mark_dirty();
}

void CoreUI::UI_Transform::set_size(float width, float height) {
	size.x = width; size.y = height;
	if (!is_dirty) mark_dirty();
}

void CoreUI::UI_Transform::set_size(glm::vec2 size) {
	this->size = size;
	if (!is_dirty) mark_dirty();
}

void CoreUI::UI_Transform::add_child(UI_Transform& transform) {
	children.emplace_back(transform.object_id);
	transform.parent_id = object_id;
	if (!transform.is_dirty) transform.mark_dirty();
}

void CoreUI::UI_Transform::translate(float x, float y) {
	local_pos.x += x; local_pos.y += y;
	mark_dirty();
}

void CoreUI::UI_Transform::translate(glm::vec2 vec) {
	local_pos += vec;
	mark_dirty();
}

void CoreUI::UI_Transform::update_transform() {
	if (parent_id != 0) global_pos = UI_ObjectManager::get_instance().get(parent_id)->transform.global_pos + local_pos;
	else global_pos = local_pos;
	is_dirty = false;
}

void CoreUI::UI_Transform::mark_dirty() {
	if (is_dirty) return;
	is_dirty = true;
	UI_ObjectManager::get_instance().mark_dirty_transform(object_id);
	for (auto& child_id : children) {
		UI_Transform& tr = UI_ObjectManager::get_instance().get(child_id)->transform;
		if(!tr.is_dirty) tr.mark_dirty();
	}
}