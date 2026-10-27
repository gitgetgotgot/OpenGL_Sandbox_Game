#include "UI/UI_Object.h"
#include "UI/UI_ObjectManager.h"
#include "UI/UI_CompManager.h"
#include "UI/UI_BehaviourSystem.h"

Engine::UI::UI_Object::UI_Object(uint32_t id) {
	object_id = id;
	transform.object_id = id;
}

void Engine::UI::UI_Object::add_child(UI_Object* child) {
	return transform.add_child(child->transform);
}

void Engine::UI::UI_Object::add_behaviour(void* ptr, UI_Behaviour_VTable* vt) const {
	UI_BehaviourSystem::get_instance().add(object_id, ptr, vt);
}


Engine::UI::UI_Obj_Ptr::UI_Obj_Ptr(const UI_Obj_Ptr& other) noexcept {
	if (this != &other) {
		this->obj_id = other.obj_id;
	}
}

Engine::UI::UI_Object* Engine::UI::UI_Obj_Ptr::operator->() const {
	return UI_ObjectManager::get_instance().get(obj_id);
}

Engine::UI::UI_Object& Engine::UI::UI_Obj_Ptr::operator*() const {
	return *UI_ObjectManager::get_instance().get(obj_id);
}

Engine::UI::UI_Obj_Ptr::operator bool() const {
	return obj_id != 0 && UI_ObjectManager::get_instance().get(obj_id);
}

Engine::UI::UI_Obj_Ptr& Engine::UI::UI_Obj_Ptr::operator=(const UI_Obj_Ptr& other) noexcept {
	if (this != &other) {
		this->obj_id = other.obj_id;
	}
	return *this;
}

Engine::UI::UI_Obj_Ptr& Engine::UI::UI_Obj_Ptr::operator=(UI_Obj_Ptr&& other) noexcept {
	if (this != &other) {
		this->obj_id = other.obj_id;
		other.obj_id = 0;
	}
	return *this;
}

bool Engine::UI::UI_Obj_Ptr::add_child(UI_Obj_Ptr& child) const {
	if (obj_id == child.obj_id) return false;
	auto obj = UI_ObjectManager::get_instance().get(obj_id);
	auto child_obj = UI_ObjectManager::get_instance().get(child.obj_id);
	if (!obj || !child_obj) return false;
	obj->add_child(child_obj);
	return true;
}

Engine::UI::UI_Obj_Ptr Engine::UI::UI_Obj_Ptr::get_child_by_index(uint32_t index) const {
	auto obj = UI_ObjectManager::get_instance().get(obj_id);
	return UI_Obj_Ptr(obj->transform.children[index]);
}