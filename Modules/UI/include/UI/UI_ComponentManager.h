#pragma once
#include "UI/UI_Component.h"
#include <Utility/SparseSet.h>
#include <memory>

namespace CoreUI {
	class UI_ComponentManager {
	public:
		static UI_ComponentManager& get_instance() {
			static UI_ComponentManager mgr;
			return mgr;
		}
		void ClearData();
		UI_Component* add(std::unique_ptr<UI_Component> comp);
		bool remove(uint32_t ui_object_id);
		UI_Component* get(uint32_t ui_object_id);
		uint32_t size();
	private:
		UI_ComponentManager() {}
		~UI_ComponentManager() {}
		sparse_set<std::unique_ptr<UI_Component>> components;
		std::vector<uint32_t> dirty_comps;
	};
}