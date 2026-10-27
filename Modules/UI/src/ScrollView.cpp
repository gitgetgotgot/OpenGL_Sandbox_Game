#include "UI/ScrollView.h"
#include "UI/UI_ObjectManager.h"

void Engine::UI::ScrollView::set_group_layout(GroupLayout layout) {
	this->layout = layout;
}

void Engine::UI::ScrollView::set_border_offsets(float offset_left, float offset_top) {
	border_x_offset = offset_left;
	border_y_offset = offset_top;
}

void Engine::UI::ScrollView::set_content_children_offset(float children_offset) {
	content_children_offset = children_offset;
}

void Engine::UI::ScrollView::set_mouse_wheel_scroll_speed(float speed) {
	mouse__wheel_scroll_speed = speed;
}

void Engine::UI::ScrollView::bind_content(UI_Component_Ptr<ScrollViewContent>& c) {
	content = c;
}

void Engine::UI::ScrollView::bind_vertical_scrollbar(UI_Component_Ptr<ScrollBar>& sb) {
	this->scrollbar_Y = sb;
	scrollbar_Y->on_value_changed.set_callback_method<ScrollViewContent, &ScrollViewContent::_set_normalized_offset_y>(content.operator->());
}

void Engine::UI::ScrollView::bind_horizontal_scrollbar(UI_Component_Ptr<ScrollBar>& sb) {
	this->scrollbar_X = sb;
	scrollbar_X->on_value_changed.set_callback_method<ScrollViewContent, &ScrollViewContent::_set_normalized_offset_x>(content.operator->());
}

void Engine::UI::ScrollView::update_content() {
	//amount of children is changed -> replace all children
	auto& scroll_view_content = *content;
	auto content_obj = UI_ObjectManager::get_instance().get(content.get_id());
	if (UI_Transform& tr = content_obj->transform; scroll_view_content.last_children_size != tr.children.size()) {
		scroll_view_content.last_children_size = tr.children.size();

		//get left and top corners of view
		const float left_corner = tr.local_pos.x - tr.size.x * tr.pivot.x;
		const float top_corner = tr.local_pos.y + tr.size.y * tr.pivot.y;

		// setup initial offsets with alignment at Top Left corner
		float current_x_offset = left_corner + border_x_offset;
		float current_y_offset = top_corner - border_y_offset;

		float max_element_x_size = 0.0f;
		float max_element_y_size = 0.0f;

		const bool is_horizontal = layout == ScrollView::GroupLayout::LAYOUT_HORIZONTAL;
		const bool is_vertical = layout == ScrollView::GroupLayout::LAYOUT_VERTICAL;

		for (auto& child_id : tr.children) {
			UI_Transform& child_tr = UI_ObjectManager::get_instance().get(child_id)->transform;
			child_tr.set_local_pos(
				current_x_offset + child_tr.size.x * 0.5f,
				current_y_offset - child_tr.size.y * 0.5f
			);
			if (is_horizontal) current_x_offset += child_tr.size.x + content_children_offset;
			if (is_vertical)   current_y_offset -= child_tr.size.y + content_children_offset;

			max_element_x_size = child_tr.size.x > max_element_x_size ? child_tr.size.x : max_element_x_size;
			max_element_y_size = child_tr.size.y > max_element_y_size ? child_tr.size.y : max_element_y_size;
		}
		current_x_offset += border_x_offset;
		current_y_offset -= border_y_offset;

		// adjust offsets for axis
		if (!is_horizontal) current_x_offset += max_element_x_size;
		if (!is_vertical) current_y_offset -= max_element_y_size;

		// count total content width and height
		const float total_content_width = current_x_offset - left_corner; // x_offset will be positive and left_corner is negative
		const float total_content_height = top_corner - current_y_offset; // y_offset will be negative and top_corner is positive

		// max content X offset is negative and counts as difference between <view width> and <total content width>
		// max content Y offset is positive and counts as difference between <total content height> and <view width>
		scroll_view_content.current_max_content_offset.x = tr.size.x - total_content_width;
		scroll_view_content.current_max_content_offset.y = total_content_height - tr.size.y;

		// max content X offset can't be positive and max content Y offset can't be negative (happens if content size is less than view size)
		if (scroll_view_content.current_max_content_offset.x > 0.0f) scroll_view_content.current_max_content_offset.x = 0.0f;
		if (scroll_view_content.current_max_content_offset.y < 0.0f) scroll_view_content.current_max_content_offset.y = 0.0f;

		if (has_horizontal_scrollbar && scrollbar_X) {
			scrollbar_X->set_thumb_size_ratio(tr.size.x / total_content_width);
		}
		if (has_vertical_scrollbar && scrollbar_Y) {
			scrollbar_Y->set_thumb_size_ratio(tr.size.y / total_content_height);
		}
	}
}

void Engine::UI::ScrollView::force_update_content() {
	content->last_children_size = 0;
	update_content();
}