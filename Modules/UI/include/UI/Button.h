#pragma once
#include "UI/UI_Comp_Ptr.h"
#include "UI/SDF_Text.h"
#include "UI/Image.h"
#include <Utility/FunctionWrapper.h>

namespace CoreUI {
	class Button : public UI_Component<Button> {
	public:
		Button() = default;
		Button(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_BUTTON, UI_Render_Type::UI_NONE) {
		}
		~Button() = default;

		void bind_components(
			UI_Component_Ptr<Image>& button_image,
			UI_Component_Ptr<SDF_Text>& button_text
		);
		void set_color_base(glm::vec4 color);
		void set_color_hover(glm::vec4 color);
		void set_color_press(glm::vec4 color);

		void _on_cursor_enter();
		void _on_cursor_exit();
		void _on_cursor_pressed();
		void _on_callback_use();
		bool _is_pointed = false;
		bool _is_pressed = false;

		FunctionWrapper on_button_click;
		FunctionWrapper on_cursor_enter;
		FunctionWrapper on_cursor_exit;
		UI_Component_Ptr<Image> button_image;
		UI_Component_Ptr<SDF_Text> button_text;
	private:
		glm::vec4 color_base{ 0.9f, 0.9f, 0.9f, 1.0f };
		glm::vec4 color_hover{ 1.0f };
		glm::vec4 color_press{ 0.7f, 0.7f, 0.7f, 1.0f };
	};
}