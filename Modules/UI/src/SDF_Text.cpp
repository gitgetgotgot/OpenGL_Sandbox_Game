#include "UI/SDF_Text.h"
#include "UI/UI_ObjectManager.h"
#include "UI/FontManager.h"

void CoreUI::SDF_Text::_update_buffer_data(UI_RenderContext& c, UI_RenderState& s) {
	if (text.empty()) return;
	size_t text_size = text.size();
	update_render_queue(c, s, 6 * text_size);
	c.sdf_text_buffer.insert(c.sdf_text_buffer.end(), text_vertices.begin(), text_vertices.begin() + text_size * 4);
}

void CoreUI::SDF_Text::_update_data() {
	is_dirty = false;
	if (text.empty()) return;
	UI_Object* obj = UI_ObjectManager::get_instance().get(object_id);

	if (dirty_mask & TextMask::TEXT_TRANSFORM) {
		UI_Transform& tr = obj->transform;
		Font& font = FontManager::get_instance().get_main_font();
		float start_x = tr.global_pos.x - tr.size.x * 0.5f;
		float start_y = tr.global_pos.y + (-font.descender - 0.35f) * this->text_height;
		float x0, x1, y0, y1;
		float x_offset = 0;
		size_t text_size = text.size();

		if (align_horizontal == TextHorizAlign::Center_Align) {
			float total_text_length = this->text_height * 0.1f; //a little offset
			for (uint32_t i = 0; i < text_size; i++) {
				total_text_length += font.glyphsData[(uint32_t)text[i] - 32U].advance_normalized * this->text_height;
			}
			x_offset = (tr.size.x - total_text_length) * 0.5f;
		}
		else if (align_horizontal == TextHorizAlign::Right_Align) {
			float total_text_length = this->text_height * 0.1f; //a little offset
			for (uint32_t i = 0; i < text_size; i++) {
				total_text_length += font.glyphsData[(uint32_t)text[i] - 32U].advance_normalized * this->text_height;
			}
			x_offset = tr.size.x - total_text_length;
		}
		if (align_vertical == TextVertAlign::Top_Align) {
			start_y += (tr.size.y - text_height) * 0.5f;
		}
		else if (align_vertical == TextVertAlign::Bottom_Align) {
			start_y -= (tr.size.y - text_height) * 0.5f;
		}

		if (text_vertices.capacity() < text_size * 4) {
			text_vertices.resize(text_size * 4 + 40);
		}

		UI_Text_Vertex2f* v_buf = text_vertices.data();
		for (uint32_t i = 0; i < text_size; i++) {
			GlyphData& glyph = font.glyphsData[(uint32_t)text[i] - 32U];

			x0 = start_x + x_offset + glyph.bearingX_normalized * this->text_height;
			x1 = x0 + glyph.width_normalized * this->text_height;
			y0 = start_y + (glyph.bearingY_normalized - glyph.height_normalized) * this->text_height;
			//y0 = start_y;
			y1 = y0 + glyph.height_normalized * this->text_height;

			v_buf->pos = { x0, y0 }; v_buf->UV = { glyph.U0, glyph.V0 }; v_buf++->color = color;
			v_buf->pos = { x0, y1 }; v_buf->UV = { glyph.U0, glyph.V1 }; v_buf++->color = color;
			v_buf->pos = { x1, y1 }; v_buf->UV = { glyph.U1, glyph.V1 }; v_buf++->color = color;
			v_buf->pos = { x1, y0 }; v_buf->UV = { glyph.U1, glyph.V0 }; v_buf++->color = color;

			x_offset += this->text_height * glyph.advance_normalized;
		}
	}
	else if (dirty_mask & TextMask::TEXT_COLOR) {
		UI_Text_Vertex2f* v_buf = text_vertices.data();
		size_t size = text.size() * 4;
		for (size_t i = 0; i < size; i++) {
			v_buf++->color = color;
		}
	}

	dirty_mask = TextMask::TEXT_CLEAN;
}

void CoreUI::SDF_Text::set_text(std::string text) {
	this->text = text;
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

void CoreUI::SDF_Text::set_text(std::string_view text) {
	this->text = text;
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

void CoreUI::SDF_Text::set_text(const char* text) {
	this->text = text;
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

void CoreUI::SDF_Text::set_text_height(float height) {
	text_height = height;
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

void CoreUI::SDF_Text::set_color(float r, float g, float b, float a) {
	color.x = r; color.y = g; color.z = b; color.w = a;
	mark_dirty();
	dirty_mask |= TextMask::TEXT_COLOR;
}

void CoreUI::SDF_Text::set_color(glm::vec4 color) {
	set_color(color.x, color.y, color.z, color.w);
}

void CoreUI::SDF_Text::set_alignment(TextHorizAlign horiz, TextVertAlign vert) {
	align_horizontal = horiz; align_vertical = vert;
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

void CoreUI::SDF_Text::add_char_to_text(const char c, uint32_t pos) {
	text.insert(text.begin() + pos, c);
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

void CoreUI::SDF_Text::remove_char_from_text(uint32_t pos) {
	text.erase(text.begin() + pos);
	mark_dirty();
	dirty_mask |= TextMask::TEXT_TRANSFORM;
}

float CoreUI::SDF_Text::get_char_offset(uint32_t char_index) {
	if (text.empty()) return text_height * 0.1f;
	Font& font = FontManager::get_instance().get_main_font();
	float offset = font.glyphsData[(uint32_t)text[0] - 32U].bearingX_normalized * text_height + text_height * 0.1f;
	for (uint32_t i = 0; i < char_index; i++) {
		offset += font.glyphsData[(uint32_t)text[i] - 32U].advance_normalized * text_height;
	}
	return offset;
}