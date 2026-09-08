#pragma once
#include <cstdint>
#include <memory>
#include "UI/UI_Transform.h"
#include "UI/UI_Component.h"

namespace CoreUI {
	class UI_Object {
	public:
		UI_Object(uint32_t id);
		bool add_child(uint32_t child_object_id);
		void add_child(UI_Object* child);
		template<typename T> T* add_component();
		template<typename T> T* get_component();
		UI_Transform transform;
		uint32_t object_id;
		bool is_enabled = true;
	private:
		UI_Component* add_component(std::unique_ptr<UI_Component> comp);
		UI_Component* get_component(uint32_t id);
	};

	template<typename T>
	inline T* UI_Object::add_component() {
		static_assert(std::is_base_of_v<UI_Component, T>, "T must inherit from UI_Component");
		std::unique_ptr<UI_Component> new_comp = std::make_unique<T>(object_id);
		return static_cast<T*>(add_component(std::move(new_comp)));
	}

	template<typename T>
	inline T* UI_Object::get_component() {
		static_assert(std::is_base_of_v<UI_Component, T>, "T must inherit from UI_Component");
		return static_cast<T*>(get_component(object_id));
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
		uint32_t get_id() const;
	private:
		uint32_t obj_id = 0;
	};
}