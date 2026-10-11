#pragma once
#include "UI/UI_Comp_Ptr.h"
#include "UI/Image.h"
#include <Utility/FunctionWrapper.h>

namespace CoreUI {
	class ScrollBar : public UI_Component<ScrollBar> {
		friend class UI_BehaviourSystem;
	public:
		enum ScrollDirection : uint8_t { SCROLLBAR_VERTICAL, SCROLLBAR_HORIZONTAL };
		enum ScrollMode : uint8_t { SLIDER_MODE, PAGEABLE_MODE };
		ScrollBar() = default;
		ScrollBar(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_SCROLL_BAR, UI_Render_Type::UI_NONE) {
		}
		~ScrollBar() = default;

		void set_value(float value);
		void set_direction(ScrollDirection direction);
		void set_scroll_mode(ScrollMode mode);
		void set_steps_amount(uint16_t steps);
		void set_thumb_size_ratio(float ratio);
		void bind_components(UI_Component_Ptr<Image>& track_img, UI_Component_Ptr<Image>& thumb_img);

		FunctionWrapper<float> on_value_changed;
		float scroll_value = 0.0f;
		bool is_discrete = true;
	private:
		void _update_thumb_pos();
		void _on_track_held();
		void _on_thumb_drag(float thumb_x, float thumb_y, float prev_mouse_ortho_x, float prev_mouse_ortho_y);
		void _on_thumb_step(float thumb_x, float thumb_y);

		bool _is_dragged = false;
		ScrollDirection direction = ScrollDirection::SCROLLBAR_HORIZONTAL;
		ScrollMode scroll_mode = ScrollMode::SLIDER_MODE;

		UI_Component_Ptr<Image> track_img;
		UI_Component_Ptr<Image> thumb_img;
		float thumb_step_size = 0.01f;
		float max_thumb_range = 0.0f;
		float min_thumb_offset = 0.0f;
		float scroll_raw_offset = 0.0f;
	};
}