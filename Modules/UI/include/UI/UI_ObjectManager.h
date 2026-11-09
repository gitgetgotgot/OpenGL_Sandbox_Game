#pragma once
#include "UI/UI_Object.h"
#include <Utility/SparseSet.h>

namespace Engine::UI {
	class UI_ObjectManager {
	public:
		static UI_ObjectManager& Instance() {
			static UI_ObjectManager mgr;
			return mgr;
		}
		void ClearData();
		UI_Obj_Ptr add();
		void destroy(uint32_t ui_object_id);
		void mark_destroyed_object(uint32_t ui_object_id);
		UI_Object* get(uint32_t ui_object_id);
		void mark_dirty_transform(uint32_t ui_object_id);
		void mark_dirty_component(uint32_t ui_object_id);
		void update_dirty_objects();
		void remove_destroyed_objects();
	private:
		UI_ObjectManager() {}
		~UI_ObjectManager() {}
		Engine::Util::sparse_set<UI_Object> objects;
		std::vector<uint32_t> free_ids;
		uint32_t current_max_id = 1;
		std::vector<uint32_t> dirty_transforms;
		std::vector<uint32_t> dirty_components;
		std::vector<uint32_t> destroyed_objects;
	};
}