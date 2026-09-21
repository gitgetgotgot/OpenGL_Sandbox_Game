#include "UI/Image.h"
#include "UI/UI_ObjectManager.h"
#include <Resources/Sprite.h>

void CoreUI::Image::_update_sprite_buffer(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Vertex2f>& sprites_buffer,
	uint16_t clip_rect_id
) {
	if (sprite_ID == 0) return;
	update_render_queue(render_queue, clip_rect_id, INDEX_SIZE);
	sprites_buffer.insert(sprites_buffer.end(), image_vertices.data, image_vertices.data + image_vertices.size); // THINK ABOUT std::memcpy!!!
}

void CoreUI::Image::_update_render_data() {
	is_dirty = false;
	if (sprite_ID == 0) return;
	data_updater(*this);
	dirty_mask = ImageMask::IMAGE_CLEAN;
}

void CoreUI::Image::Setup_As_Regular() {
	image_type = ImageType::RegularImage;
	INDEX_SIZE = 6;
	image_vertices = DynamicArray<UI_Vertex2f>(4);
	data_updater = &Image::update_as_regular;
}

void CoreUI::Image::Setup_As_Nine_Sliced() {
	image_type = ImageType::NineSlicedImage;
	INDEX_SIZE = 54;
	image_vertices = DynamicArray<UI_Vertex2f>(36);
	data_updater = &Image::update_as_nine_sliced;
}

void CoreUI::Image::set_sprite(uint32_t sprite_ID) {
	this->sprite_ID = sprite_ID;
	mark_dirty();
	dirty_mask |= ImageMask::IMAGE_SPRITE;
}

void CoreUI::Image::set_color(glm::vec4& color) {
	this->image_color = color;
	mark_dirty();
	dirty_mask |= ImageMask::IMAGE_COLOR;
}

void CoreUI::Image::set_flip(bool X, bool Y) {
	flip_x = X; flip_y = Y;
	mark_dirty();

}

void CoreUI::Image::set_border_size(float normalized_size) {
	this->border_normalized_size = normalized_size;
	mark_dirty();
	dirty_mask |= ImageMask::IMAGE_BORDER;
}

void CoreUI::Image::update_as_regular(Image& img) {
	UI_Object* obj = UI_ObjectManager::get_instance().get(img.object_id);
	UI_Vertex2f* vertex = img.image_vertices.data;

	if (img.dirty_mask & ImageMask::IMAGE_TRANSFORM || img.dirty_mask & ImageMask::IMAGE_SPRITE) {
		UI_Transform& tr = obj->transform;
		CoreResource::Sprite& sprite = CoreResource::SpriteManager::get_instance().get_sprite(img.sprite_ID);

		float size_x = tr.size.x;
		float size_y = tr.size.y;
		float x = tr.global_pos.x - size_x * 0.5f;
		float y = tr.global_pos.y - size_y * 0.5f;

		glm::vec2& size_ratio = sprite.ratio;
		x += (1.0f - size_ratio.x) * size_x * 0.5f;
		y += (1.0f - size_ratio.y) * size_y * 0.5f;
		size_x *= size_ratio.x;
		size_y *= size_ratio.y;
		
		UI_Vertex2f& v1 = vertex[0];
		v1.pos = { x, y };						v1.UV = { sprite.U0, sprite.V0 };						v1.tex_index = sprite.texture_id;
		UI_Vertex2f& v2 = vertex[1];
		v2.pos = { x, y + size_y };				v2.UV = { sprite.U0, sprite.V0 + sprite.H };			v2.tex_index = sprite.texture_id;
		UI_Vertex2f& v3 = vertex[2];
		v3.pos = { x + size_x, y + size_y };	v3.UV = { sprite.U0 + sprite.W, sprite.V0 + sprite.H }; v3.tex_index = sprite.texture_id;
		UI_Vertex2f& v4 = vertex[3];
		v4.pos = { x + size_x, y };				v4.UV = { sprite.U0 + sprite.W, sprite.V0 };			v4.tex_index = sprite.texture_id;
	}
	if (img.dirty_mask & ImageMask::IMAGE_COLOR) {
		vertex[0].color = img.image_color;
		vertex[1].color = img.image_color;
		vertex[2].color = img.image_color;
		vertex[3].color = img.image_color;
	}
}

void CoreUI::Image::update_as_nine_sliced(Image& img) {
	UI_Object* obj = UI_ObjectManager::get_instance().get(img.object_id);
	UI_Vertex2f* v_buf = nullptr;

	if (img.dirty_mask & ImageMask::IMAGE_TRANSFORM || img.dirty_mask & ImageMask::IMAGE_BORDER) {
		CoreResource::Sprite9Sliced& sprite = CoreResource::SpriteManager::get_instance().get_sprite9sliced(img.sprite_ID);
		float margin_left	= sprite.ratio_left * img.border_normalized_size;
		float margin_right	= sprite.ratio_right * img.border_normalized_size;
		float margin_top	= sprite.ratio_top * img.border_normalized_size;
		float margin_bottom	= sprite.ratio_bottom * img.border_normalized_size;

		UI_Transform& tr = obj->transform;
		float
			X_0 = tr.global_pos.x - tr.size.x * 0.5f, X_1 = X_0 + tr.size.x,
			Y_0 = tr.global_pos.y - tr.size.y * 0.5f, Y_1 = Y_0 + tr.size.y;
		float
			x0 = X_0, x1 = X_0 + margin_left,
			y0 = Y_0, y1 = Y_0 + margin_bottom;
		v_buf = img.image_vertices.data;

		auto update_buffer_transform_part = [&]() {
			v_buf++->pos = { x0, y0 };
			v_buf++->pos = { x0, y1 };
			v_buf++->pos = { x1, y1 };
			v_buf++->pos = { x1, y0 };
		};

		update_buffer_transform_part(); //LB

		y0 = y1, y1 = Y_1 - margin_top,
		update_buffer_transform_part(); //LC

		y0 = y1, y1 = Y_1,
		update_buffer_transform_part(); //LT

		x0 = x1, x1 = X_1 - margin_right,
		y0 = Y_0, y1 = Y_0 + margin_bottom,
		update_buffer_transform_part(); //CB

		y0 = y1, y1 = Y_1 - margin_top,
		update_buffer_transform_part(); //C

		y0 = y1, y1 = Y_1,
		update_buffer_transform_part(); //CT

		x0 = x1, x1 = X_1,
		y0 = Y_0, y1 = Y_0 + margin_bottom,
		update_buffer_transform_part(); //RB

		y0 = y1, y1 = Y_1 - margin_top,
		update_buffer_transform_part(); //RC

		y0 = y1, y1 = Y_1,
		update_buffer_transform_part(); //RT
	}
	if (img.dirty_mask & ImageMask::IMAGE_SPRITE) {
		CoreResource::Sprite9Sliced& sprite = CoreResource::SpriteManager::get_instance().get_sprite9sliced(img.sprite_ID);
		float
			U_0 = sprite.U0, U_1 = U_0 + sprite.W,
			V_0 = sprite.V0, V_1 = V_0 + sprite.H;
		float
			u0 = U_0, u1 = u0 + sprite.uv_margin_left,
			v0 = V_0, v1 = v0 + sprite.uv_margin_bottom;
		v_buf = img.image_vertices.data;

		auto update_buffer_sprite_part = [&]() {
			v_buf->UV = { u0, v0 }; v_buf++->tex_index = sprite.texture_id;
			v_buf->UV = { u0, v1 }; v_buf++->tex_index = sprite.texture_id;
			v_buf->UV = { u1, v1 }; v_buf++->tex_index = sprite.texture_id;
			v_buf->UV = { u1, v0 }; v_buf++->tex_index = sprite.texture_id;
		};

		update_buffer_sprite_part(); //LB

		v0 = v1, v1 = V_1 - sprite.uv_margin_top;
		update_buffer_sprite_part(); //LC

		v0 = v1, v1 = V_1;
		update_buffer_sprite_part(); //LT

		u0 = u1, u1 = U_1 - sprite.uv_margin_right,
		v0 = V_0, v1 = v0 + sprite.uv_margin_bottom;
		update_buffer_sprite_part(); //CB

		v0 = v1, v1 = V_1 - sprite.uv_margin_top;
		update_buffer_sprite_part(); //C

		v0 = v1, v1 = V_1;
		update_buffer_sprite_part(); //CT

		u0 = u1, u1 = U_1,
		v0 = V_0, v1 = v0 + sprite.uv_margin_bottom;
		update_buffer_sprite_part(); //RB

		v0 = v1, v1 = V_1 - sprite.uv_margin_top;
		update_buffer_sprite_part(); //RC

		v0 = v1, v1 = V_1;
		update_buffer_sprite_part(); //RT
	}
	if (img.dirty_mask & ImageMask::IMAGE_COLOR) {
		v_buf = img.image_vertices.data;
		for (uint32_t i = 0; i < img.image_vertices.size; i++) {
			v_buf++->color = img.image_color;
		}
	}
}