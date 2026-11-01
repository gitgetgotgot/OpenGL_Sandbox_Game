#pragma once
#include "UI/Canvas.h"

namespace Engine::UI {
	class CanvasManager {
		struct Canvas_Sort_Data {
			uint32_t index;
			uint32_t layer_index;
			bool operator<(const Canvas_Sort_Data& other) const { return layer_index < other.layer_index; }
		};
	public:
		static CanvasManager& Instance() {
			static CanvasManager mgr;
			return mgr;
		}
		void ClearData();
		void update_render_queue(UI_RenderContext& ctx, UI_RenderState state);
		UI_Canvas_Ptr add();
		Canvas* get(uint32_t id);
		void mark_canvases_order_dirty();
	private:
		CanvasManager() {}
		~CanvasManager() {}
		std::vector<Canvas> canvases;
		std::vector<Canvas_Sort_Data> canvas_sorted_queue;
		bool resort_canvases = false;
	};
}