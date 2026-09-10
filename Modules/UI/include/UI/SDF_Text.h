#pragma once
#include "UI/UI_Component.h"
#include <string>

namespace CoreUI {
	class SDF_Text : public UI_Component {
	public:
		SDF_Text(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_WIDGET, UI_Render_Type::UI_TEXT) {}
		virtual void _update_sdf_text_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
			uint16_t clip_rect_id
		) override;
		virtual void _update_render_data() override;
		void set_text(std::string text);
		void set_text(std::string_view text);
		void set_text(const char* text);
		void set_text_height(float height);
		void set_color(float r, float g, float b, float a);
		void set_color(glm::vec4 color);
		std::string text{};
		glm::vec4 color{ 1.0f };
		float text_height = 0.1f;
	private:
		std::vector<UI_Text_Vertex2f> text_vertices;
	};
}