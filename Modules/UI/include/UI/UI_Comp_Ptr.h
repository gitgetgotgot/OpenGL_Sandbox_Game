#pragma once
#include "UI/UI_ComponentManager.h"

namespace CoreUI {
	template<typename T>
	class UI_Component_Ptr {
	public:
		UI_Component_Ptr() {}
		UI_Component_Ptr(uint32_t obj_id) : obj_id{ obj_id } {}
		T* operator->() const;
		T& operator*() const;
		operator bool() const;
		UI_Component_Ptr<T>& operator=(const UI_Component_Ptr<T>&) noexcept;
		UI_Component_Ptr<T>& operator=(UI_Component_Ptr<T>&&) noexcept;
		uint32_t get_id() const { return obj_id; }
		void reset() { obj_id = 0; }
	private:
		uint32_t obj_id = 0;
	};
	template<typename T>
	inline T* UI_Component_Ptr<T>::operator->() const {
		return static_cast<T*>(UI_ComponentManager::get_instance().get(obj_id)->component);
	}

	template<typename T>
	inline T& UI_Component_Ptr<T>::operator*() const {
		return *static_cast<T*>(UI_ComponentManager::get_instance().get(obj_id)->component);
	}

	template<typename T>
	inline UI_Component_Ptr<T>::operator bool() const {
		return obj_id != 0 && UI_ComponentManager::get_instance().get(obj_id);
	}

	template<typename T>
	inline UI_Component_Ptr<T>& UI_Component_Ptr<T>::operator=(const UI_Component_Ptr<T>& other) noexcept {
		if (this != &other) {
			this->obj_id = other.obj_id;
		}
		return *this;
	}

	template<typename T>
	inline UI_Component_Ptr<T>& UI_Component_Ptr<T>::operator=(UI_Component_Ptr<T>&& other) noexcept {
		if (this != &other) {
			this->obj_id = other.obj_id;
			other.obj_id = 0;
		}
		return *this;
	}
}