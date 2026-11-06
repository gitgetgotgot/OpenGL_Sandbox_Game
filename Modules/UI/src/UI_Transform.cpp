#include "UI/UI_Transform.h"
#include "UI/UI_ObjectManager.h"
#include "UI/UI_CompManager.h"

void Engine::UI::UI_Transform::set_anchor(Anchor anchor) {
	this->anchor = anchor;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_local_pos(float x, float y) {
	local_pos.x = x; local_pos.y = y;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_local_pos(glm::vec2 pos) {
	local_pos = pos;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_size(float width, float height) {
	size.x = width; size.y = height;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_size(glm::vec2 size) {
	this->size = size;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_pivot(float pivot_x, float pivot_y) {
	pivot.x = pivot_x, pivot.y = pivot_y;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_pivot(glm::vec2 pivot) {
	this->pivot = pivot;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_width(float width) {
	size.x = width;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::set_height(float height) {
	size.y = height;
	if (!is_dirty) mark_dirty();
}

void Engine::UI::UI_Transform::add_child(UI_Transform& transform) {
	children.emplace_back(transform.object_id);
	transform.parent_id = object_id;
	if (!transform.is_dirty) transform.mark_dirty();
}

bool Engine::UI::UI_Transform::remove_child(UI_Transform& transform) {
	return remove_child(transform.object_id);
}

bool Engine::UI::UI_Transform::remove_child(uint32_t child_id) {
	const size_t size = children.size();
	for (size_t i = 0; i < size; ++i) {
		if (children[i] == child_id) {
			children.erase(children.begin() + i);
			return true;
		}
	}
	return false;
}

void Engine::UI::UI_Transform::translate(float x, float y) {
	local_pos.x += x; local_pos.y += y;
	mark_dirty();
}

void Engine::UI::UI_Transform::translate(glm::vec2 vec) {
	local_pos += vec;
	mark_dirty();
}

void Engine::UI::UI_Transform::force_update() {
	if (is_dirty) {
		update_transform();
		UI_ObjectManager::Instance().unmark_dirty_transform(object_id);
	}
}

void Engine::UI::UI_Transform::update_transform() {
	if (parent_id != 0) {
		UI_Transform& parent_tr = UI_ObjectManager::Instance().get(parent_id)->transform;
		//global_pos = parent_tr.global_pos + local_pos + parent_tr.size * ANCHOR_MULTIPLIERS[static_cast<size_t>(anchor)];
		global_pos = parent_tr.global_pos + local_pos;
	}
	else {
		global_pos = local_pos;
}
	is_dirty = false;
}

void Engine::UI::UI_Transform::mark_dirty() {
	if (is_dirty) return;
	is_dirty = true;
	UI_ComponentEntry* comp = UI_ComponentManager::Instance().get(object_id);
	if (comp) static_cast<UI_ComponentBase*>(comp->component)->dirty_mask |= COMPONENT_TRANSFORM;

	UI_ObjectManager::Instance().mark_dirty_transform(object_id);
	for (auto& child_id : children) {
		UI_Transform& tr = UI_ObjectManager::Instance().get(child_id)->transform;
		if(!tr.is_dirty) tr.mark_dirty();
	}
}

void Engine::UI::UI_Transform::mark_destroyed() {
	if (is_destroyed) return;
	is_destroyed = true;
	UI_ObjectManager::Instance().mark_destroyed_object(object_id);
	for (auto& child_id : children) {
		UI_Transform& tr = UI_ObjectManager::Instance().get(child_id)->transform;
		if (!tr.is_dirty) tr.mark_destroyed();
	}
}