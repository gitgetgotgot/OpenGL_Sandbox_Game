#pragma once
#include "UI/UI_Comp_Ptr.h"

namespace CoreUI {
	class Image : public UI_Component<Image> {
	public:
		Image() = default;
		Image(uint32_t object_id) :
			UI_Component<Image>(object_id, UI_Component_Type::UI_WIDGET, UI_Render_Type::UI_SPRITE) {}
		~Image() = default;

		void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			uint16_t clip_rect_id
		);
		void _update_render_data();

		void set_sprite(uint32_t sprite_ID);
		void flip(bool X, bool Y);

		uint32_t sprite_ID = 0;
		bool flip_x = false, flip_y = false;
	private:
		UI_Vertex2f sprite_vertices[4];
	};
}