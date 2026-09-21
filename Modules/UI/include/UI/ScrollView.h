#pragma once
#include "UI/UI_Comp_Ptr.h"

namespace CoreUI {
	class ScrollView : public UI_Component<ScrollView> {
	public:
		ScrollView() = default;
		ScrollView(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_SCROLL_VIEW, UI_Render_Type::UI_NONE) {
		}
		~ScrollView() = default;
		glm::vec2& get_content_offset();
	private:
		bool _is_pointed = false;
		bool _is_pressed = false;
		glm::vec2 content_offset{ 0.0f };
	};
}