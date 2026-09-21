#pragma once
#include "UI/UI_Comp_Ptr.h"
#include <Utility/DynamicArray.h>

namespace CoreUI {
	class Image : public UI_Component<Image> {
	public:
		enum ImageMask : uint8_t {
			IMAGE_CLEAN = 0,
			IMAGE_TRANSFORM = 1 << 0,
			IMAGE_COLOR = 1 << 1,
			IMAGE_SPRITE = 1 << 2,
			IMAGE_BORDER = 1 << 3
		};
		enum ImageType : uint8_t { RegularImage, NineSlicedImage };
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

		void Setup_As_Regular();
		void Setup_As_Nine_Sliced();
		void set_sprite(uint32_t sprite_ID);
		void set_color(glm::vec4& color);
		void set_flip(bool X, bool Y);
		void set_border_size(float normalized_size);

		glm::vec4 image_color{ 1.0f };
		uint32_t sprite_ID = 0;
		float border_normalized_size = 0.03f;
		ImageType image_type = ImageType::RegularImage;
		uint8_t INDEX_SIZE = 6;
		bool flip_x = false, flip_y = false;
	private:
		void (*data_updater)(Image& img);
		DynamicArray<UI_Vertex2f> image_vertices;
		static void update_as_regular(Image& img);
		static void update_as_nine_sliced(Image& img);
	};
}