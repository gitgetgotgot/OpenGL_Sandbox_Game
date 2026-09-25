#pragma once
#include "UI/UI_Component.h"
#include <string>

namespace CoreUI {
	class SDF_Text : public UI_Component<SDF_Text> {
		friend class InputField;
	public:
		enum TextMask : uint8_t {
			TEXT_CLEAN = 0,
			TEXT_TRANSFORM = 1 << 0,
			TEXT_COLOR = 1 << 1
		};
		enum TextVertAlign : uint8_t { Bottom_Align, Middle_Align, Top_Align };
		enum TextHorizAlign : uint8_t { Left_Align, Center_Align, Right_Align };

		SDF_Text() = default;
		SDF_Text(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_WIDGET, UI_Render_Type::UI_TEXT) {}
		~SDF_Text() = default;

		void _update_buffer_data(UI_RenderContext& c, UI_RenderState& s);
		void _update_data();

		void set_text(std::string text);
		void set_text(std::string_view text);
		void set_text(const char* text);
		void set_text_height(float height);
		void set_color(float r, float g, float b, float a);
		void set_color(glm::vec4 color);
		void set_alignment(TextHorizAlign horiz, TextVertAlign vert);

		std::string text{};
		glm::vec4 color{ 1.0f };
		float text_height = 0.1f;
		TextVertAlign align_vertical = TextVertAlign::Middle_Align;
		TextHorizAlign align_horizontal = TextHorizAlign::Left_Align;
	private:
		std::vector<UI_Text_Vertex2f> text_vertices;
		void add_char_to_text(const char c, uint32_t pos);
		void remove_char_from_text(uint32_t pos);
		float get_char_offset(uint32_t char_index);
	};
}