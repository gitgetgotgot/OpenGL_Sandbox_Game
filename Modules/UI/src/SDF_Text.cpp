#include "UI/SDF_Text.h"
#include "UI/UI_ObjectManager.h"
#include <Resources/FontManager.h>

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
		CoreResource::Font& font = CoreResource::FontManager::Instance().get_font(text_font_id);
		size_t text_size = text.size();

		// get text lines
		const float MAX_LINE_LENGTH = tr.size.x;
		std::vector<TextLineInfo> text_lines;
		uint32_t line_start_index = 0;
		uint32_t last_part_end_index = 0;
		float current_line_length = 0.0f;
		float last_part_length = 0.0f;
		for (uint32_t i = 0; i < text_size; ++i) {
			const char& c = text[i];
			if (c == '\0') { //just for safety
				text_size = i;
				text.erase(i);
				break;
			}
			if (c == ' ' || c == '\t') { // SPACE or TAB found -> last_part_length is known
				if (c == '\t')
					last_part_length += 4.0f * font.glyphsData[0].advance_normalized * text_height; //use 4 times space advance
				else
					last_part_length += font.glyphsData[0].advance_normalized * text_height; //use space advance

				current_line_length += last_part_length;
				last_part_length = 0.0f;
				last_part_end_index = i; // remember end index of part where TAB or SPACE is located currently
			}
			else if (c == '\n') { //end of line or string -> place new line
				current_line_length += last_part_length;
				text_lines.emplace_back(TextLineInfo{ std::string_view(text.begin() + line_start_index, text.begin() + i), current_line_length });
				current_line_length = 0.0f;
				last_part_length = 0.0f;
				line_start_index = i + 1;
			}
			else { // any other glyph -> just take its advance and add to part_length
				last_part_length += font.glyphsData[(uint32_t)c - 32U].advance_normalized * text_height;
			}

			if (current_line_length > 0.0f && current_line_length + last_part_length >= MAX_LINE_LENGTH) {
				text_lines.emplace_back(TextLineInfo{ std::string_view(text.begin() + line_start_index, text.begin() + last_part_end_index), current_line_length });
				line_start_index = last_part_end_index + 1;
				current_line_length = 0.0f;
			}
		}
		if (line_start_index < text_size) {
			current_line_length += last_part_length;
			text_lines.emplace_back(TextLineInfo{ std::string_view(text.begin() + line_start_index, text.begin() + text_size), current_line_length });
		}

		// count total text height and resize widget logic height if needed
		const float text_line_height = (font.ascender - font.descender) * text_height;
		const float text_TOTAL_HEIGHT = text_lines.size() * text_line_height;
		if (adaptive_widget_height) {
			tr.set_height(text_TOTAL_HEIGHT + 0.02f); //set widget height as its text height + some offset (optional)
		}

		float start_x, start_y;
		float x0, x1, y0, y1;
		float x_offset = 0;
		float y_offset = 0;

		// change buffer capacity if needed
		if (text_vertices.capacity() < text_size * 4) {
			text_vertices.resize(text_size * 4 + 40);
		}

		start_y = tr.global_pos.y + font.descender * text_height;
		if (align_vertical == TextVertAlign::Top_Align) {
			start_y += tr.size.y * tr.pivot.y - text_line_height * 0.5f;
		}
		else if (align_vertical == TextVertAlign::Bottom_Align) {
			start_y += -tr.size.y * tr.pivot.y + text_TOTAL_HEIGHT - text_line_height * 0.5f;
		}
		else {
			start_y += (text_TOTAL_HEIGHT - text_line_height) * 0.5f;
		}

		// build text buffer using lines
		UI_Text_Vertex2f* v_buf = text_vertices.data();
		for (auto& line_info : text_lines) {
			if (align_horizontal == TextHorizAlign::Center_Align) {
				start_x = tr.global_pos.x - line_info.total_length * tr.pivot.x;
			}
			else if (align_horizontal == TextHorizAlign::Right_Align) {
				start_x = tr.global_pos.x + tr.size.x * tr.pivot.x - line_info.total_length;
			}
			else {
				start_x = tr.global_pos.x - tr.pivot.x * tr.size.x;
			}
			x_offset = 0.0f;
			for (auto& c : line_info.line) {
				if (c == '\t') {
					x_offset += font.glyphsData[0].advance_normalized * text_height * 4.0f;
					continue;
				}
				CoreResource::Font::GlyphData& glyph = font.glyphsData[(uint32_t)c - 32U];

				x0 = start_x + x_offset + glyph.bearingX_normalized * text_height;
				x1 = x0 + glyph.width_normalized * text_height;

				y0 = start_y + y_offset + (glyph.bearingY_normalized - glyph.height_normalized) * text_height;
				y1 = y0 + glyph.height_normalized * text_height;

				v_buf->pos = { x0, y0 }; v_buf->UV = { glyph.U0, glyph.V0 }; v_buf++->color = color;
				v_buf->pos = { x0, y1 }; v_buf->UV = { glyph.U0, glyph.V1 }; v_buf++->color = color;
				v_buf->pos = { x1, y1 }; v_buf->UV = { glyph.U1, glyph.V1 }; v_buf++->color = color;
				v_buf->pos = { x1, y0 }; v_buf->UV = { glyph.U1, glyph.V0 }; v_buf++->color = color;

				x_offset += text_height * glyph.advance_normalized;
			}
			y_offset -= text_line_height;
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

void CoreUI::SDF_Text::set_text(std::string& text) {
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

void CoreUI::SDF_Text::force_update() {
	if (is_dirty) {
		_update_data();
		UI_ObjectManager::get_instance().unmark_dirty_component(object_id);
		UI_ObjectManager::get_instance().get(object_id)->transform.force_update();
	}
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
	CoreResource::Font& font = CoreResource::FontManager::Instance().get_font(text_font_id);
	float offset = font.glyphsData[(uint32_t)text[0] - 32U].bearingX_normalized * text_height + text_height * 0.1f;
	for (uint32_t i = 0; i < char_index; i++) {
		offset += font.glyphsData[(uint32_t)text[i] - 32U].advance_normalized * text_height;
	}
	return offset;
}