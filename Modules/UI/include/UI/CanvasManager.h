#pragma once
#include "UI/Canvas.h"

namespace Engine::UI {
	class CanvasManager {
	public:
		static CanvasManager& get_instance() {
			static CanvasManager mgr;
			return mgr;
		}
		void ClearData();
		UI_Canvas_Ptr add();
		Canvas* get(uint32_t id);
		std::vector<Canvas>& get_canvases();
	private:
		CanvasManager() {}
		~CanvasManager() {}
		std::vector<Canvas> canvases;
	};
}