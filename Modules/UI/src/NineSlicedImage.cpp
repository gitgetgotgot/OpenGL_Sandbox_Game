#include "UI/NineSlicedImage.h"
#include "UI/UI_ObjectManager.h"
#include <Utility/Sprite.h>

void CoreUI::NineSlicedImage::set_sprite9sliced(uint32_t sprite_ID) {
	this->sprite_ID = sprite_ID;
	mark_dirty();
}

void CoreUI::NineSlicedImage::set_frame_size(float left, float right, float top, float bottom) {
	frame.left = left, frame.right = right, frame.top = top, frame.bottom = bottom;
	mark_dirty();
}

void CoreUI::NineSlicedImage::_update_sprite_buffer(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	uint16_t clip_rect_id
) {
	update_render_queue(render_queue, clip_rect_id, 54);
	sprites_buffer.insert(sprites_buffer.end(), sprite_vertices, sprite_vertices + 36);
}

void CoreUI::NineSlicedImage::_update_render_data() {
	is_dirty = false;
	if (sprite_ID == 0) return;

	UI_Object* obj = UI_ObjectManager::get_instance().get(object_id);
	if (!obj) return;

	UI_Transform& tr = obj->transform;
	float	X_0 = tr.global_pos.x - tr.size.x * 0.5f, X_1 = X_0 + tr.size.x,
			Y_0 = tr.global_pos.y - tr.size.y * 0.5f, Y_1 = Y_0 + tr.size.y;
	float	x0 = X_0, x1 = X_0 + frame.left,
			y0 = Y_0, y1 = Y_0 + frame.bottom;

	CoreResource::Sprite9Sliced& sprite = CoreResource::SpriteManager::get_instance().get_sprite9sliced(sprite_ID);
	float	U_0 = sprite.U0, U_1 = U_0 + sprite.W,
			V_0 = sprite.V0, V_1 = V_0 + sprite.H;
	float	u0 = U_0, u1 = u0 + sprite.b_left,
			v0 = V_0, v1 = v0 + sprite.b_bottom;

	UI_Vertex2f* buf_ptr = sprite_vertices;
	auto update_buffer_part = [&]() {
		*buf_ptr++ = UI_Vertex2f(x0, y0, u0, v0, sprite.texture_id);
		*buf_ptr++ = UI_Vertex2f(x0, y1, u0, v1, sprite.texture_id);
		*buf_ptr++ = UI_Vertex2f(x1, y1, u1, v1, sprite.texture_id);
		*buf_ptr++ = UI_Vertex2f(x1, y0, u1, v0, sprite.texture_id);
	};

	// left bottom
	update_buffer_part();

	//left center
	y0 = y1, y1 = Y_1 - frame.top,
	v0 = v1, v1 = V_1 - sprite.b_top;
	update_buffer_part();

	//left top
	y0 = y1, y1 = Y_1,
	v0 = v1, v1 = V_1;
	update_buffer_part();

	//center bottom
	x0 = x1, x1 = X_1 - frame.right,
	u0 = u1, u1 = U_1 - sprite.b_right,
	y0 = Y_0, y1 = Y_0 + frame.bottom,
	v0 = V_0, v1 = v0 + sprite.b_bottom;
	update_buffer_part();
	
	//center
	y0 = y1, y1 = Y_1 - frame.top,
	v0 = v1, v1 = V_1 - sprite.b_top;
	update_buffer_part();

	//center top
	y0 = y1, y1 = Y_1,
	v0 = v1, v1 = V_1;
	update_buffer_part();

	//right bottom
	x0 = x1, x1 = X_1,
	u0 = u1, u1 = U_1,
	y0 = Y_0, y1 = Y_0 + frame.bottom,
	v0 = V_0, v1 = v0 + sprite.b_bottom;
	update_buffer_part();

	//right center
	y0 = y1, y1 = Y_1 - frame.top,
	v0 = v1, v1 = V_1 - sprite.b_top;
	update_buffer_part();

	//right top
	y0 = y1, y1 = Y_1,
	v0 = v1, v1 = V_1;
	update_buffer_part();
}