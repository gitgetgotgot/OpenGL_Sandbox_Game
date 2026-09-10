#include "UI/CanvasManager.h"

void CoreUI::CanvasManager::ClearData() {
	canvases = {};
}

CoreUI::UI_Canvas_Ptr CoreUI::CanvasManager::add() {
	uint32_t id = canvases.size();
	canvases.emplace_back();
	return UI_Canvas_Ptr(id);
}

CoreUI::Canvas* CoreUI::CanvasManager::get(uint32_t id) {
	return &canvases[id];
}

std::vector<CoreUI::Canvas>& CoreUI::CanvasManager::get_canvases() {
	return canvases;
}