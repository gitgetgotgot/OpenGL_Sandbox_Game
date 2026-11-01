#include "UI/CanvasManager.h"
#include <algorithm>

void Engine::UI::CanvasManager::ClearData() {
	canvases = {};
}

void Engine::UI::CanvasManager::update_render_queue(UI_RenderContext& ctx, UI_RenderState state) {
	if (resort_canvases) {
		resort_canvases = false;
		canvas_sorted_queue.clear();
		uint32_t size = canvases.size();
		for (uint32_t i = 0; i < size; ++i) {
			canvas_sorted_queue.emplace_back(Canvas_Sort_Data{ i, canvases[i].layer_index });
		}
		std::stable_sort(canvas_sorted_queue.begin(), canvas_sorted_queue.end());
	}

	for (auto& [index, layer] : canvas_sorted_queue) {
		Canvas& canvas = canvases[index];
		if (canvas.is_enabled) canvas.update_canvas_objects_data(ctx, state);
	}
}

Engine::UI::UI_Canvas_Ptr Engine::UI::CanvasManager::add() {
	uint32_t id = canvases.size();
	canvases.emplace_back();
	resort_canvases = true;
	return UI_Canvas_Ptr(id);
}

Engine::UI::Canvas* Engine::UI::CanvasManager::get(uint32_t id) {
	return &canvases[id];
}

void Engine::UI::CanvasManager::mark_canvases_order_dirty() {
	resort_canvases = true;
}