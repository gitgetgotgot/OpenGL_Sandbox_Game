#pragma once
#include "UI/UI_Transform.h"
#include "UI/UI_Comp_Ptr.h"
#include "UI/UI_Behaviour.h"

namespace CoreUI {
	class UI_Object {
	public:
		UI_Object(uint32_t id);
		void add_child(UI_Object* child);
		template<typename T> UI_Component_Ptr<T> add_component();
		template<typename T> UI_Component_Ptr<T> get_component();
		template<typename T> T* add_component_behaviour();
		UI_Transform transform;
		uint32_t object_id;
		bool is_enabled = true;
	private:
		void add_behaviour(void* ptr, UI_Behaviour_VTable* vt) const;
	};

	template<typename T>
	inline UI_Component_Ptr<T> UI_Object::add_component() {
		static_assert(std::is_base_of_v<UI_Component<T>, T>, "T must inherit from UI_Component<T>");
		void* comp = new T(object_id);
		UI_ComponentManager::get_instance().add(comp, T::get_vt(), object_id);
		return UI_Component_Ptr<T>(object_id);
	}

	template<typename T>
	inline UI_Component_Ptr<T> UI_Object::get_component() {
		static_assert(std::is_base_of_v<UI_Component, T>, "T must inherit from UI_Component");
		return UI_Component_Ptr<T>(object_id);
	}

	template<typename T>
	inline T* UI_Object::add_component_behaviour() {
		static_assert(std::is_base_of_v<UI_Behaviour<T>, T>, "T must inherit from UI_Behaviour<T>");
		void* obj = new T();
		add_behaviour(obj, T::get_vt());
		return static_cast<T*>(obj);
	}

	class UI_Obj_Ptr {
	public:
		UI_Obj_Ptr() {}
		UI_Obj_Ptr(uint32_t obj_id) : obj_id{ obj_id } {}
		UI_Object* operator->() const;
		UI_Object& operator*() const;
		operator bool() const;
		UI_Obj_Ptr& operator=(const UI_Obj_Ptr&) noexcept;
		UI_Obj_Ptr& operator=(UI_Obj_Ptr&&) noexcept;
		bool add_child(UI_Obj_Ptr& child) const;
		uint32_t get_id() const { return obj_id; }
		void reset() { obj_id = 0; }
	private:
		uint32_t obj_id = 0;
	};
}