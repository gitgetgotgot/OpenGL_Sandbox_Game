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

void CoreUI::ScrollView::bind_vertical_scrollbar(UI_Component_Ptr<ScrollBar>& sb) {
	this->scrollbar_Y = sb;
	scrollbar_Y->on_value_changed.set_callback_method<ScrollView, &ScrollView::_set_normalized_offset_y>(this);
}

void CoreUI::ScrollView::bind_horizontal_scrollbar(UI_Component_Ptr<ScrollBar>& sb) {
	this->scrollbar_X = sb;
	scrollbar_X->on_value_changed.set_callback_method<ScrollView, &ScrollView::_set_normalized_offset_x>(this);
}

void CoreUI::ScrollView::_set_normalized_offset_x(float value) {
	content_offset.x = value * current_max_content_offset.x;
}

void CoreUI::ScrollView::_set_normalized_offset_y(float value) {
	content_offset.y = value * current_max_content_offset.y;
}