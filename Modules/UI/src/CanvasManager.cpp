#include "UI/CanvasManager.h"

void Engine::UI::CanvasManager::ClearData() {
	canvases = {};
}

Engine::UI::UI_Canvas_Ptr Engine::UI::CanvasManager::add() {
	uint32_t id = canvases.size();
	canvases.emplace_back();
	return UI_Canvas_Ptr(id);
}

Engine::UI::Canvas* Engine::UI::CanvasManager::get(uint32_t id) {
	return &canvases[id];
}

std::vector<Engine::UI::Canvas>& Engine::UI::CanvasManager::get_canvases() {
	return canvases;
}