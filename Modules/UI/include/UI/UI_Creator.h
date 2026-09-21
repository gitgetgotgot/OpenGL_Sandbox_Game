#pragma once
#include "UI/UI_Object.h"
#include "UI/Canvas.h"

namespace CoreUI {
	class UI_Creator {
	public:
		static UI_Creator& Instance() {
			static UI_Creator creator;
			return creator;
		}
		UI_Canvas_Ptr create_canvas();

		UI_Obj_Ptr create_ui_empty_object();

		UI_Obj_Ptr create_image();
		UI_Obj_Ptr create_image(uint32_t sprite_id, glm::vec2 size, glm::vec2 local_pos);

		UI_Obj_Ptr create_image9sliced();
		UI_Obj_Ptr create_image9sliced(uint32_t sprite9sliced_id, glm::vec2 size, glm::vec2 local_pos);

		UI_Obj_Ptr create_SDF_text();
		UI_Obj_Ptr create_SDF_text(glm::vec2 size, glm::vec2 local_pos, std::string text, glm::vec4 color);

		UI_Obj_Ptr create_panel();
		UI_Obj_Ptr create_panel(glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, glm::vec4 color);

		UI_Obj_Ptr create_button();
		UI_Obj_Ptr create_button_image(
			glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, glm::vec4 text_color, float text_height
		);
		UI_Obj_Ptr create_button_image9sliced(
			glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, glm::vec4 text_color, float text_height
		);

		UI_Obj_Ptr create_input_field();
		UI_Obj_Ptr create_input_field_image(
			glm::vec2 size, glm::vec2 local_pos, uint32_t back_sprite_id, glm::vec4 text_color, float text_height
		);
		UI_Obj_Ptr create_input_field_image9sliced(
			glm::vec2 size, glm::vec2 local_pos, uint32_t back_sprite9sliced_id, glm::vec4 text_color, float text_height
		);
	private:
		UI_Creator() = default;
		~UI_Creator() = default;
	};
}