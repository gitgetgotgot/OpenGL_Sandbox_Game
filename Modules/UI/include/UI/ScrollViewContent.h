#pragma once
#include "UI/UI_Comp_Ptr.h"

namespace CoreUI {
	class ScrollViewContent : public UI_Component<ScrollViewContent> {
		friend class UI_BehaviourSystem;
		friend class ScrollView;
		friend class Canvas;
	public:
		ScrollViewContent() = default;
		ScrollViewContent(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_SCROLL_VIEW_CONTENT, UI_Render_Type::UI_NONE) {
		}
		~ScrollViewContent() = default;
	private:
		void _set_normalized_offset_x(float value);
		void _set_normalized_offset_y(float value);
	private:
		uint16_t last_children_size = 0;
		glm::vec2 content_offset{ 0.0f };
		glm::vec2 current_max_content_offset{ 0.0f };
	};
}