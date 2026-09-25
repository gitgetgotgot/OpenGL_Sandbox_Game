#include "UI/ScrollView.h"

glm::vec2& CoreUI::ScrollView::_get_content_offset() {
	return content_offset;
}

void CoreUI::ScrollView::set_group_layout(GroupLayout layout) {
	this->layout = layout;
}

void CoreUI::ScrollView::set_border_offsets(float offset_left, float offset_top) {
	border_x_offset = offset_left;
	border_y_offset = offset_top;
}

void CoreUI::ScrollView::set_content_children_offset(float children_offset) {
	content_children_offset = children_offset;
}

void CoreUI::ScrollView::set_mouse_wheel_scroll_speed(float speed) {
	mouse__wheel_scroll_speed = speed;
}