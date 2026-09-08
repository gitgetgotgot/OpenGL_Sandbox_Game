#include "UI/NineSlicedImage.h"
#include "UI/UI_ObjectManager.h"
#include <Utility/Sprite.h>

void CoreUI::NineSlicedImage::set_sprite9sliced(uint32_t sprite_ID) {
	this->sprite_ID = sprite_ID;
	mark_dirty();
}

void CoreUI::NineSlicedImage::flip(bool X, bool Y) {
	flip_x = X, flip_y = Y;
	mark_dirty();
}

void CoreUI::NineSlicedImage::set_frame_size(float size) {
	frame_size = size;
	mark_dirty();
}

void CoreUI::NineSlicedImage::set_inner_size(float width, float height) {
	inner_size.x = width; inner_size.y = height;
	mark_dirty();
}

void CoreUI::NineSlicedImage::set_inner_size(glm::vec2 size) {
	set_inner_size(size.x, size.y);
}

void CoreUI::NineSlicedImage::_update_sprite_buffer(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer
) {
	//index_count = 36 vertices * 6 indices
	UI_RenderEntry& last_entry = render_queue.back();
	if (last_entry.type == UI_Component_Type::UI_SPRITE)
		last_entry.index_count += 216;
	else
		UI_RenderEntry& entry = render_queue.emplace_back(UI_Component_Type::UI_SPRITE, 216);

	sprites_buffer.insert(sprites_buffer.end(), sprite_vertices, sprite_vertices + 36);
}

void CoreUI::NineSlicedImage::_update_render_data() {
	is_dirty = false;
	if (sprite_ID == 0) return;

	UI_Object* obj = UI_ObjectManager::get_instance().get(object_id);
	if (!obj) return;

	UI_Transform& tr = obj->transform;

	float x0 = tr.global_pos.x;
	float y0 = tr.global_pos.y;
	float x1 = x0 + tr.size.x;
	float y1 = y0 + tr.size.y;

	CoreResource::Sprite9Sliced& sprite = CoreResource::SpriteManager::get_instance().get_sprite9sliced(sprite_ID);
	float U0 = sprite.U0, U1 = U0 + sprite.b_left;
	float V0 = sprite.V0, V1 = V0 + sprite.b_bottom;

	UI_Vertex2f* buf_ptr = sprite_vertices;

	auto update_buffer_part = [&]() {
		*buf_ptr++ = UI_Vertex2f(x0, y0, U0, V0, sprite.texture_id);
		*buf_ptr++ = UI_Vertex2f(x0, y1, U0, V1, sprite.texture_id);
		*buf_ptr++ = UI_Vertex2f(x1, y1, U1, V1, sprite.texture_id);
		*buf_ptr++ = UI_Vertex2f(x1, y0, U1, V0, sprite.texture_id);
	};

	/*
	
	//left bottom
	update_buffer_part();
	//left center
	y0 = y1; y1 = 
	V0 = V1; V1 = sprite.V0 + sprite.H - sprite.b_top;
	update_buffer_part();
	//left top
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//center bottom
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	x0 = x1; x1 += inner_width;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//center
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//center top
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//right bottom
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	x0 = x1; x1 += tooltip_frame_size;
	y0 = start_y; y1 = y0 + tooltip_frame_size;
	add_tooltip_part_to_buffer();
	//right center
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += inner_height;
	add_tooltip_part_to_buffer();
	//right top
	sprite = &CoreResource::SpriteManager::get_instance().get_sprite(first_sprite_id++);
	y0 = y1; y1 += tooltip_frame_size;
	add_tooltip_part_to_buffer();

	*/

	
}