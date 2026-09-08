#pragma once
#include "UI/UI_Component.h"

namespace CoreUI {
	class Image : public UI_Component {
	public:
		Image(uint32_t object_id) : UI_Component(object_id) { type = UI_Component_Type::UI_SPRITE; }
		void set_sprite(uint32_t sprite_ID);
		void flip(bool X, bool Y);
		void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer
		) override;
		void _update_render_data() override;
		uint32_t sprite_ID = 0;
		bool flip_x = false, flip_y = false;
	private:
		UI_Vertex2f sprite_vertices[4];
	};
}