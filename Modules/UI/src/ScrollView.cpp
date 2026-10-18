#include "UI/ScrollView.h"
#include "UI/UI_ObjectManager.h"

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

void CoreUI::ScrollView::bind_content(UI_Component_Ptr<ScrollViewContent>& c) {
	content = c;
}

void CoreUI::ScrollView::bind_vertical_scrollbar(UI_Component_Ptr<ScrollBar>& sb) {
	this->scrollbar_Y = sb;
	scrollbar_Y->on_value_changed.set_callback_method<ScrollViewContent, &ScrollViewContent::_set_normalized_offset_y>(content.operator->());
}

void CoreUI::ScrollView::bind_horizontal_scrollbar(UI_Component_Ptr<ScrollBar>& sb) {
	this->scrollbar_X = sb;
	scrollbar_X->on_value_changed.set_callback_method<ScrollViewContent, &ScrollViewContent::_set_normalized_offset_x>(content.operator->());
}

void CoreUI::ScrollView::update_content() {
	//amount of children is changed -> replace all children
	auto& scroll_view_content = *content;
	auto content_obj = UI_ObjectManager::get_instance().get(content.get_id());
	if (UI_Transform& tr = content_obj->transform; scroll_view_content.last_children_size != tr.children.size()) {
		scroll_view_content.last_children_size = tr.children.size();
		//alignment is always Top Left
		const float initial_x_pos = tr.local_pos.x - tr.size.x * 0.5f + this->border_x_offset;
		const float initial_y_pos = tr.local_pos.y + tr.size.y * 0.5f - this->border_y_offset;

		float current_x_offset = initial_x_pos;
		float current_y_offset = initial_y_pos;

		float max_element_x_size = 0.0f;
		float max_element_y_size = 0.0f;

		const bool is_horizontal = this->layout == ScrollView::GroupLayout::LAYOUT_HORIZONTAL;
		const bool is_vertical = this->layout == ScrollView::GroupLayout::LAYOUT_VERTICAL;

		for (auto& child_id : tr.children) {
			UI_Transform& child_tr = UI_ObjectManager::get_instance().get(child_id)->transform;
			child_tr.set_local_pos(
				current_x_offset + child_tr.size.x * 0.5f,
				current_y_offset - child_tr.size.y * 0.5f
			);
			if (is_horizontal) current_x_offset += child_tr.size.x + this->content_children_offset;
			if (is_vertical)   current_y_offset -= child_tr.size.y + this->content_children_offset;

			max_element_x_size = child_tr.size.x > max_element_x_size ? child_tr.size.x : max_element_x_size;
			max_element_y_size = child_tr.size.y > max_element_y_size ? child_tr.size.y : max_element_y_size;
		}
		current_x_offset += this->border_x_offset;
		current_y_offset -= this->border_y_offset;

		if (!is_horizontal) current_x_offset += max_element_x_size;
		if (!is_vertical) current_y_offset -= max_element_y_size;

		scroll_view_content.current_max_content_offset.x = tr.local_pos.x - tr.size.x * 0.5f - current_x_offset + tr.size.x;
		scroll_view_content.current_max_content_offset.y = tr.local_pos.y + tr.size.y * 0.5f - current_y_offset - tr.size.y;

		if (scroll_view_content.current_max_content_offset.x > 0.0f) scroll_view_content.current_max_content_offset.x = 0.0f;
		if (scroll_view_content.current_max_content_offset.y < 0.0f) scroll_view_content.current_max_content_offset.y = 0.0f;

		if (this->has_horizontal_scrollbar && this->scrollbar_X && scroll_view_content.current_max_content_offset.x != 0.0f) {
			this->scrollbar_X->set_thumb_size_ratio(tr.size.x / -scroll_view_content.current_max_content_offset.x);
		}
		if (this->has_vertical_scrollbar && this->scrollbar_Y && scroll_view_content.current_max_content_offset.y != 0.0f) {
			this->scrollbar_Y->set_thumb_size_ratio(tr.size.y / scroll_view_content.current_max_content_offset.y);
		}
	}
}

void CoreUI::ScrollView::_set_normalized_offset_x(float value) {
	
}

void CoreUI::ScrollView::_set_normalized_offset_y(float value) {
	
}