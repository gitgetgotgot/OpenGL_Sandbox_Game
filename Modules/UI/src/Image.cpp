#include "UI/Image.h"
#include "UI/UI_ObjectManager.h"
#include <Resources/Sprite.h>

void CoreUI::Image::set_sprite(uint32_t sprite_ID) {
	this->sprite_ID = sprite_ID;
	mark_dirty();
}

void CoreUI::Image::flip(bool X, bool Y) {
	flip_x = X; flip_y = Y;
	mark_dirty();
}

void CoreUI::Image::_update_sprite_buffer(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	uint16_t clip_rect_id
) {
	if (sprite_ID == 0) return;
	update_render_queue(render_queue, clip_rect_id, 6);
	sprites_buffer.insert(sprites_buffer.end(), sprite_vertices, sprite_vertices + 4);
}

void CoreUI::Image::_update_render_data() {
	is_dirty = false;
	if (sprite_ID == 0) return;

	UI_Object* obj = UI_ObjectManager::get_instance().get(object_id);
	if (!obj) return;

	UI_Transform& tr = obj->transform;
	float x = tr.global_pos.x - tr.size.x * 0.5f;
	float y = tr.global_pos.y - tr.size.y * 0.5f;
	float size_x = tr.size.x;
	float size_y = tr.size.y;

	CoreResource::Sprite& sprite = CoreResource::SpriteManager::get_instance().get_sprite(sprite_ID);
	glm::vec2& size_ratio = sprite.ratio;
	x += (1.0f - size_ratio.x) * size_x * 0.5f;
	y += (1.0f - size_ratio.y) * size_y * 0.5f;
	size_x *= size_ratio.x;
	size_y *= size_ratio.y;
	sprite_vertices[0] = UI_Vertex2f(x, y,						sprite.U0, sprite.V0,						sprite.texture_id);
	sprite_vertices[1] = UI_Vertex2f(x, y + size_y,				sprite.U0, sprite.V0 + sprite.H,			sprite.texture_id);
	sprite_vertices[2] = UI_Vertex2f(x + size_x, y + size_y,	sprite.U0 + sprite.W, sprite.V0 + sprite.H, sprite.texture_id);
	sprite_vertices[3] = UI_Vertex2f(x + size_x, y,				sprite.U0 + sprite.W, sprite.V0,			sprite.texture_id);
}