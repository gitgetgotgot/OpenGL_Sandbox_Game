#include "UI/SDF_Text.h"
#include "UI/UI_ObjectManager.h"
#include "UI/FontManager.h"

void CoreUI::SDF_Text::_update_sdf_text_buffer(
	std::vector<UI_RenderEntry>& render_queue,
	std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
	uint16_t clip_rect_id
) {
	if (text.empty()) return;
	update_render_queue(render_queue, clip_rect_id, 6 * text.size());
	sdf_text_buffer.insert(sdf_text_buffer.end(), text_vertices.begin(), text_vertices.end());
}

void CoreUI::SDF_Text::_update_render_data() {
	UI_Object* obj = UI_ObjectManager::get_instance().get(object_id);
	if (!obj) return;

	UI_Transform& tr = obj->transform;
	Font& font = FontManager::get_instance().get_main_font();
	float start_x = tr.global_pos.x;
	float start_y = tr.global_pos.y - font.descender * this->text_height;
	float x0, x1, y0, y1;
	float x_offset = 0;
	uint32_t text_size = text.size();

	text_vertices.clear();
	for (uint32_t i = 0; i < text_size; i++) {
		GlyphData& glyph = font.glyphsData[(uint32_t)text[i] - 32U];

		x0 = start_x + x_offset + glyph.bearingX_normalized * this->text_height;
		x1 = x0 + glyph.width_normalized * this->text_height;
		y0 = start_y + (glyph.bearingY_normalized - glyph.height_normalized) * this->text_height;
		y1 = y0 + glyph.height_normalized * this->text_height;

		text_vertices.emplace_back(x0, y0, glyph.U0, glyph.V0, color);
		text_vertices.emplace_back(x0, y1, glyph.U0, glyph.V1, color);
		text_vertices.emplace_back(x1, y1, glyph.U1, glyph.V1, color);
		text_vertices.emplace_back(x1, y0, glyph.U1, glyph.V0, color);

		x_offset += this->text_height * glyph.advance_normalized;
	}
}

void CoreUI::SDF_Text::set_text(std::string text) {
	this->text = text;
	mark_dirty();
}

void CoreUI::SDF_Text::set_text(std::string_view text) {
	this->text = text;
	mark_dirty();
}

void CoreUI::SDF_Text::set_text(const char* text) {
	this->text = text;
	mark_dirty();
}

void CoreUI::SDF_Text::set_text_height(float height) {
	text_height = height;
	mark_dirty();
}

void CoreUI::SDF_Text::set_color(float r, float g, float b, float a) {
	color.x = r; color.y = g; color.z = b; color.w = a;
	mark_dirty();
}

void CoreUI::SDF_Text::set_color(glm::vec4 color) {
	set_color(color.x, color.y, color.z, color.w);
}