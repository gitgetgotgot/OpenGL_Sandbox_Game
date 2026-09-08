#pragma once
#include "UI/UI_Object.h"
#include <Utility/SparseSet.h>
#include <unordered_map>

namespace CoreUI {
	struct UI_Dirty_Flags {
		bool dirty_transform = false;
		bool dirty_component = false;
	};

	class UI_ObjectManager {
	public:
		static UI_ObjectManager& get_instance() {
			static UI_ObjectManager mgr;
			return mgr;
		}
		void ClearData();
		UI_Object* add();
		bool remove(uint32_t ui_object_id);
		UI_Object* get(uint32_t ui_object_id);
		void mark_dirty_transform(uint32_t ui_object_id);
		void mark_dirty_component(uint32_t ui_object_id);
		void update_dirty_objects();
	private:
		UI_ObjectManager() {}
		~UI_ObjectManager() {}
		sparse_set<UI_Object> objects;
		std::vector<uint32_t> free_ids;
		uint32_t current_max_id = 1;
		std::unordered_map<uint32_t, UI_Dirty_Flags> dirty_objects;
	};
}