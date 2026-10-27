#pragma once
#include "UI/UI_Comp_Ptr.h"

namespace Engine::UI {
	class TiledImage : public UI_Component<TiledImage> {
	public:
		enum TiledImageMask : uint8_t {
			TILED_IMAGE_CLEAN = 0,
			TILED_IMAGE_TRANSFORM = 1 << 0,
			TILED_IMAGE_COLOR = 1 << 1,
			TILED_IMAGE_SPRITE = 1 << 2,
			TILED_IMAGE_TILE = 1 << 3
		};
		TiledImage() = default;
		TiledImage(uint32_t object_id) :
			UI_Component<TiledImage>(object_id, UI_Component_Type::UI_WIDGET, UI_Render_Type::UI_TILED_SPRITE) {}
		~TiledImage() = default;

		void _update_buffer_data(UI_RenderContext& c, UI_RenderState& s);
		void _update_data();

		void set_sprite(uint32_t sprite_ID);
		void set_color(glm::vec4& color);
		void set_flip(bool X, bool Y);
		void set_tile(float width, float height);

		glm::vec4 image_color{ 1.0f };
		uint32_t sprite_ID = 0;
		glm::vec2 tile{ 1.0f };
		bool flip_x = false, flip_y = false;
	private:
		UI_Vertex2f image_vertices[4];
	};
}