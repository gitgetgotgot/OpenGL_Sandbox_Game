#pragma once
#include "UI/UI_Behaviour.h"
#include <UI/UI_Object.h>
#include <Utility/SparseSet.h>

namespace CoreUI {
	struct UI_BehaviourEntry {
		void* obj;
		UI_Behaviour_VTable* vt;
		UI_BehaviourBase* Base() { return static_cast<UI_BehaviourBase*>(obj); }
		~UI_BehaviourEntry() {
			if (obj) vt->destroy(obj);
		}
	};
	
	class UI_BehaviourSystem {
	public:
		static UI_BehaviourSystem& get_instance() {
			static UI_BehaviourSystem sys;
			return sys;
		}
		void update();
		UI_BehaviourEntry& add(uint32_t ui_object_id, void* ptr, UI_Behaviour_VTable* vt);
		UI_BehaviourEntry* get(uint32_t ui_object_id);
		bool remove(uint32_t ui_object_id);
		std::vector<UI_Object*>& get_hit_queue();
	private:
		UI_BehaviourSystem() {}
		~UI_BehaviourSystem() {}
		sparse_set<UI_BehaviourEntry> behaviours;
		std::vector<UI_Object*> hit_queue;
	};
}