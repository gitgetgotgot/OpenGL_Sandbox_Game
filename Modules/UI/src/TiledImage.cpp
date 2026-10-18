#include "UI/TiledImage.h"
#include "UI/UI_ObjectManager.h"
#include <Resources/Sprite.h>

void CoreUI::TiledImage::_update_buffer_data(UI_RenderContext& c, UI_RenderState& s) {
	if (sprite_ID == 0) return;
	update_render_queue(c, s, 6);
	c.sprites_buffer.insert(c.sprites_buffer.end(), image_vertices, image_vertices + 4);
}

void CoreUI::TiledImage::_update_data() {
	is_dirty = false;
	if (sprite_ID == 0) return;

	UI_Object* obj = UI_ObjectManager::get_instance().get(object_id);
	if (dirty_mask & TiledImageMask::TILED_IMAGE_TRANSFORM) {
		UI_Transform& tr = obj->transform;
		const float size_x = tr.size.x;
		const float size_y = tr.size.y;
		const float x = tr.global_pos.x - size_x * tr.pivot.x;
		const float y = tr.global_pos.y - size_y * tr.pivot.y;
		image_vertices[0].pos = { x, y };
		image_vertices[1].pos = { x, y + size_y };
		image_vertices[2].pos = { x + size_x, y + size_y };
		image_vertices[3].pos = { x + size_x, y };
	}
	if (dirty_mask & TiledImageMask::TILED_IMAGE_SPRITE) {
		CoreResource::Sprite& s = CoreResource::SpriteManager::Instance().get_sprite(sprite_ID);
		UI_Vertex2f& v1 = image_vertices[0];	v1.tex_array_id = s.tex_array_id; v1.tex_id = s.tex_id;
		UI_Vertex2f& v2 = image_vertices[1];	v2.tex_array_id = s.tex_array_id; v2.tex_id = s.tex_id;
		UI_Vertex2f& v3 = image_vertices[2];	v3.tex_array_id = s.tex_array_id; v3.tex_id = s.tex_id;
		UI_Vertex2f& v4 = image_vertices[3];	v4.tex_array_id = s.tex_array_id; v4.tex_id = s.tex_id;
	}
	if (dirty_mask & TiledImageMask::TILED_IMAGE_COLOR) {
		image_vertices[0].color = image_color;
		image_vertices[1].color = image_color;
		image_vertices[2].color = image_color;
		image_vertices[3].color = image_color;
	}
	if (dirty_mask & TiledImageMask::TILED_IMAGE_TILE) {
		image_vertices[0].UV = { 0.0f, 0.0f };
		image_vertices[1].UV = { 0.0f, tile.y };
		image_vertices[2].UV = { tile.x, tile.y };
		image_vertices[3].UV = { tile.x, 0.0f };
	}
	dirty_mask = TiledImageMask::TILED_IMAGE_CLEAN;
}

void CoreUI::TiledImage::set_sprite(uint32_t sprite_ID) {
	this->sprite_ID = sprite_ID;
	mark_dirty();
	dirty_mask |= TiledImageMask::TILED_IMAGE_SPRITE;
}

void CoreUI::TiledImage::set_color(glm::vec4& color) {
	this->image_color = color;
	mark_dirty();
	dirty_mask |= TiledImageMask::TILED_IMAGE_COLOR;
}

void CoreUI::TiledImage::set_flip(bool X, bool Y) {

}

void CoreUI::TiledImage::set_tile(float width, float height) {
	tile.x = width, tile.y = height;
	mark_dirty();
	dirty_mask |= TiledImageMask::TILED_IMAGE_TILE;
}