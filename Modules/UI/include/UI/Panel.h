#pragma once
#include "UI/UI_Comp_Ptr.h"

namespace CoreUI {
	class Panel : public UI_Component {
	public:
		Panel(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_PANEL, UI_Render_Type::UI_SPRITE) {}
		void set_sprite(uint32_t sprite_ID);
		void set_color(glm::vec4 color);
		void set_color(float r, float g, float b, float a);
		void flip(bool X, bool Y);
		void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			uint16_t clip_rect_id
		) override;
		void _update_render_data() override;
		glm::vec4 color{ 1.0f };
		uint32_t sprite_ID = 0;
		bool flip_x = false, flip_y = false;
	private:
		UI_Vertex2f sprite_vertices[4];
	};
}