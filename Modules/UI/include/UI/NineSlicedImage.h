#pragma once
#include "UI/UI_Component.h"

namespace CoreUI {
	class NineSlicedImage : public UI_Component<NineSlicedImage> {
		struct FrameSize {
			float left = 0.05f, right = 0.05f,
			top = 0.05f, bottom = 0.05f;
		};
	public:
		NineSlicedImage() = default;
		NineSlicedImage(uint32_t object_id) :
			UI_Component(object_id, UI_Component_Type::UI_WIDGET, UI_Render_Type::UI_SPRITE) {}
		~NineSlicedImage() = default;

		void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			uint16_t clip_rect_id
		);
		void _update_render_data();

		void set_sprite9sliced(uint32_t sprite_ID);
		void set_frame_size(float left, float right, float top, float bottom);

		uint32_t sprite_ID = 0;
		FrameSize frame;
	private:
		UI_Vertex2f sprite_vertices[36]; //9 sprites
	};
}