#pragma once
#include "UI/UI_Component.h"
#include <Utility/SparseSet.h>

namespace CoreUI {
	class UI_ComponentEntry {
	public:
		UI_ComponentEntry() = default;
		UI_ComponentEntry(void* comp_ptr, UI_Component_VTable* comp_vt) : component{ comp_ptr }, vt{ comp_vt } {}

		UI_ComponentEntry(const UI_ComponentEntry&) = delete;
		UI_ComponentEntry& operator=(const UI_ComponentEntry&) = delete;

		UI_ComponentEntry(UI_ComponentEntry&& other) noexcept
			: component{ other.component }, vt{ other.vt } {
			other.component = nullptr;
			other.vt = nullptr;
		}
		UI_ComponentEntry& operator=(UI_ComponentEntry&& other) noexcept {
			if (this != &other) {
				if (component) {
					vt->destroy(component);
				}
				component = other.component;
				vt = other.vt;
				other.component = nullptr;
				other.vt = nullptr;
			}
			return *this;
		}

		~UI_ComponentEntry() {
			if (component) {
				vt->destroy(component); //call object destructor
				component = nullptr;
			}
		}
		void* component = nullptr;
		UI_Component_VTable* vt = nullptr;
	};

	class UI_ComponentManager {
	public:
		static UI_ComponentManager& get_instance() {
			static UI_ComponentManager mgr;
			return mgr;
		}
		void ClearData();
		//adds entry for a new component
		UI_ComponentEntry* add(void* comp, UI_Component_VTable* comp_vt, uint32_t ui_object_id);
		bool remove(uint32_t ui_object_id);
		UI_ComponentEntry* get(uint32_t ui_object_id);
		uint32_t size();
	private:
		UI_ComponentManager() {}
		~UI_ComponentManager() {}
		sparse_set<UI_ComponentEntry> components;
	};
}