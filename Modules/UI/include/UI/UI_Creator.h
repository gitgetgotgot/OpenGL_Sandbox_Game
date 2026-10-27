#pragma once
#include "UI/UI_Object.h"
#include "UI/Canvas.h"
#include <string>

namespace Engine::UI {
	class UI_Creator {
	public:
		static UI_Canvas_Ptr create_canvas();

		static UI_Obj_Ptr create_ui_empty_object();

		static UI_Obj_Ptr create_image();
		static UI_Obj_Ptr create_image(uint32_t sprite_id, glm::vec2 size, glm::vec2 local_pos, glm::vec4 color);

		static UI_Obj_Ptr create_image9sliced();
		static UI_Obj_Ptr create_image9sliced(uint32_t sprite9sliced_id, glm::vec2 size, glm::vec2 local_pos, glm::vec4 color);

		static UI_Obj_Ptr create_tiled_image();
		static UI_Obj_Ptr create_tiled_image(uint32_t sprite_id, glm::vec2 size, glm::vec2 local_pos, glm::vec2 tiles_count, glm::vec4 color);

		static UI_Obj_Ptr create_SDF_text();
		static UI_Obj_Ptr create_SDF_text(glm::vec2 size, glm::vec2 local_pos, std::string text, glm::vec4 text_color, float text_height);

		static UI_Obj_Ptr create_panel();
		static UI_Obj_Ptr create_panel(glm::vec2 size, glm::vec2 local_pos, uint32_t sprite_id, glm::vec4 color);

		static UI_Obj_Ptr create_button();
		static UI_Obj_Ptr create_button_image(
			glm::vec2 size, glm::vec2 local_pos, bool image_is_9_sliced, uint32_t sprite_id, bool has_text = false,
			std::string txt = "Button", glm::vec4 text_color = glm::vec4(1.0f), float text_height = 0.05f
		);

		static UI_Obj_Ptr create_input_field();
		// border size is for proper inner area size for text
		static UI_Obj_Ptr create_input_field_image(
			glm::vec2 size, glm::vec2 local_pos, bool image_is_9_sliced, uint32_t back_sprite_id,
			glm::vec4 text_color, float text_height, float border_size, float sliced_img_border_size = 0.03f
		);

		static UI_Obj_Ptr create_scroll_view();
		static UI_Obj_Ptr create_scroll_view(glm::vec2 size, glm::vec2 local_pos);

		static UI_Obj_Ptr create_scroll_bar();
		// slider_mode = false -> pageable_mode, vertical = false -> horizontal
		static UI_Obj_Ptr create_scroll_bar(
			glm::vec2 scroll_area_size, glm::vec2 track_size, glm::vec2 thumb_size, glm::vec2 local_pos,
			bool track_9_sliced, bool thumb_9_sliced, uint32_t track_sprite_id, uint32_t thumb_sprite_id,
			glm::vec4 track_color, glm::vec4 thumb_color,
			bool vertical, bool slider_mode, bool is_discrete = false, uint16_t discrete_steps = 0
		);
	private:
		UI_Creator() = default;
		~UI_Creator() = default;
	};
}