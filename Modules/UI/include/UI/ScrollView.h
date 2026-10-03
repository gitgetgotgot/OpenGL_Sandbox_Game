#pragma once
#include "UI/UI_Comp_Ptr.h"
#include "UI/ScrollBar.h"

namespace CoreUI {
	class ScrollView : public UI_Component<ScrollView> {
		friend class UI_BehaviourSystem;
	public:
		enum GroupLayout : uint8_t { LAYOUT_VERTICAL, LAYOUT_HORIZONTAL, LAYOUT_GRID };
		ScrollView() = default;
		ScrollView(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_SCROLL_VIEW, UI_Render_Type::UI_NONE) {
		}
		~ScrollView() = default;

		glm::vec2& _get_content_offset();

		void set_group_layout(GroupLayout layout);
		void set_border_offsets(float offset_left, float offset_top);
		void set_content_children_offset(float children_offset);
		void set_mouse_wheel_scroll_speed(float speed);
		void bind_vertical_scrollbar(UI_Component_Ptr<ScrollBar>& sb);
		void bind_horizontal_scrollbar(UI_Component_Ptr<ScrollBar>& sb);

		bool has_vertical_scrollbar = false;
		bool has_horizontal_scrollbar = false;
	private:
		void _set_normalized_offset_x();
		void _set_normalized_offset_y();

		bool _is_pointed = false;
		bool _is_pressed = false;
		bool layout_is_dirty = false;
		GroupLayout layout = GroupLayout::LAYOUT_VERTICAL;
		uint16_t last_children_size = 0;

		float border_x_offset = 0.01f;
		float border_y_offset = 0.01f;
		float content_children_offset = 0.01f;

		float mouse__wheel_scroll_speed = 0.05f;

		glm::vec2 content_offset{ 0.0f };
		glm::vec2 current_max_content_offset{ 0.0f };

		UI_Component_Ptr<ScrollBar> scrollbar_X;
		UI_Component_Ptr<ScrollBar> scrollbar_Y;
	};
}