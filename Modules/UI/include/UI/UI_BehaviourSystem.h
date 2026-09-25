#pragma once
#include "UI/UI_Behaviour.h"
#include <UI/UI_Object.h>
#include <Utility/SparseSet.h>

namespace CoreUI {
	class UI_BehaviourEntry {
	public:
		UI_BehaviourEntry() = default;
		UI_BehaviourEntry(void* behaviour_ptr, UI_Behaviour_VTable* vt_ptr) : behaviour{ behaviour_ptr }, vt{ vt_ptr } {}

		UI_BehaviourEntry(const UI_BehaviourEntry&) = delete;
		UI_BehaviourEntry& operator=(const UI_BehaviourEntry&) = delete;

		UI_BehaviourEntry(UI_BehaviourEntry&& other) noexcept
			: behaviour{ other.behaviour }, vt{ other.vt } {
			other.behaviour = nullptr;
			other.vt = nullptr;
		}
		UI_BehaviourEntry& operator=(UI_BehaviourEntry&& other) noexcept {
			if (this != &other) {
				if (behaviour) {
					vt->destroy(behaviour);
				}
				behaviour = other.behaviour;
				vt = other.vt;
				other.behaviour = nullptr;
				other.vt = nullptr;
			}
			return *this;
		}

		~UI_BehaviourEntry() {
			if (behaviour) {
				vt->destroy(behaviour);
				behaviour = nullptr;
			}
		}

		void* behaviour;
		UI_Behaviour_VTable* vt;
	};
	
	class UI_BehaviourSystem {
	public:
		static UI_BehaviourSystem& get_instance() {
			static UI_BehaviourSystem sys;
			return sys;
		}
		void update(UI_RenderContext& ctx);
		UI_BehaviourEntry* add(uint32_t ui_object_id, void* ptr, UI_Behaviour_VTable* vt);
		UI_BehaviourEntry* get(uint32_t ui_object_id);
		bool remove(uint32_t ui_object_id);
	private:
		UI_BehaviourSystem() {}
		~UI_BehaviourSystem() {}

		void process_BUTTON_event(UI_ComponentBase*& comp_base, bool& pointed_now);
		void process_INPUT_FIELD_event(UI_ComponentBase* comp_base, bool& pointed_now);
		void process_SCROLL_VIEW_event(UI_ComponentBase*& comp_base, bool& pointed_now, UI_Transform& tr);

		sparse_set<UI_BehaviourEntry> behaviours;
		uint32_t focused_component_id = 0;
		uint32_t prev_focused_id = 0;
		bool focus_changed_in_frame = false;
	};
}