#pragma once
#include "UI/UI_Component.h"

namespace CoreUI {
	class NineSlicedImage : public UI_Component {
	public:
		NineSlicedImage(uint32_t object_id) : UI_Component(object_id) {}
		void set_sprite9sliced(uint32_t sprite_ID);
		void flip(bool X, bool Y);
		void set_frame_size(float size);
		void set_inner_size(float width, float height);
		void set_inner_size(glm::vec2 size);
		void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer
		) override;
		void _update_render_data() override;
		uint32_t sprite_ID = 0;
		bool flip_x = false, flip_y = false;
		float frame_size = 0.1f;
		glm::vec2 inner_size{ 0.9f };
	private:
		UI_Vertex2f sprite_vertices[36]; //9 sprites
	};
}