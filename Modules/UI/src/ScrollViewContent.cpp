#include "UI/ScrollViewContent.h"

void Engine::UI::ScrollViewContent::_set_normalized_offset_x(float value) {
	content_offset.x = value * current_max_content_offset.x;
}

void Engine::UI::ScrollViewContent::_set_normalized_offset_y(float value) {
	content_offset.y = value * current_max_content_offset.y;
}