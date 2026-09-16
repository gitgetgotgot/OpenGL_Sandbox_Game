#pragma once
#include "UI/UI_Comp_Ptr.h"
#include "UI/Image.h"
#include "UI/SDF_Text.h"

namespace CoreUI {
	class InputField : public UI_Component<InputField> {
	public:
		InputField() = default;
		InputField(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_INPUT_FIELD, UI_Render_Type::UI_NONE) {
		}
		~InputField() = default;

		void bind_components(
			UI_Component_Ptr<SDF_Text>& text,
			UI_Component_Ptr<Image>& cursor
		);
		std::string& get_input_text() const;

		void _update_cursor();
		void _on_focus_get();
		void _on_focus_lose();
		void _on_char_input(uint32_t code);
		void _on_char_remove();
		void _on_cursor_position_changed(bool backwards);

		float cursor_blink_cd = 0.5f;
		UI_Component_Ptr<SDF_Text> text_comp;
		UI_Component_Ptr<Image> cursor_comp;
	private:
		float cursor_initial_x_pos;
		float current_cursor_cd = 0.0f;
		uint16_t cursor_position = 0;
		bool is_in_focus = false;
		bool cursor_visible = false;
	};
}