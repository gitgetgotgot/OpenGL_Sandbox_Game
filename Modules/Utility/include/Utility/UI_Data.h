#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

namespace CoreUI {
	struct UI_Vertex2f {
		UI_Vertex2f() {}
		UI_Vertex2f(float pX, float pY, float UVx, float UVy, uint32_t tex_index) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
			this->tex_index = tex_index;
		}
		UI_Vertex2f(float pX, float pY, float UVx, float UVy, uint32_t tex_index, glm::vec4 color) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
			this->color = color;
			this->tex_index = tex_index;
		}
		glm::vec2 pos{ 0.0f };
		glm::vec2 UV{ 0.0f };
		glm::vec4 color{ 1.0f };
		uint32_t tex_index = 0;
	};

	struct UI_Text_Vertex2f {
		UI_Text_Vertex2f() {}
		UI_Text_Vertex2f(float pX, float pY, float UVx, float UVy) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
		}
		UI_Text_Vertex2f(float pX, float pY, float UVx, float UVy, glm::vec4 color) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
			this->color = color;
		}
		glm::vec2 pos{ 0.0f };
		glm::vec2 UV{ 0.0f };
		glm::vec4 color{ 1.0f };
	};

	struct UI_UBO {
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::vec2 content_offset;
	};

	enum UI_Component_Type : uint8_t {
		UI_WIDGET, UI_PANEL, UI_SCROLL_VIEW, UI_INPUT_FIELD, UI_BUTTON
	};

	enum UI_Render_Type : uint8_t {
		UI_NONE, UI_SPRITE, UI_TEXT
	};

	struct ClipRectangle {
		ClipRectangle() {}
		ClipRectangle(
			uint32_t x, uint32_t y, uint32_t w, uint32_t h,
			float ortho_x, float ortho_y, float ortho_size_x, float ortho_size_y
		) :
			x{ x }, y{ y }, w{ w }, h{ h },
			ortho_x{ ortho_x }, ortho_y{ ortho_y }, ortho_size_x{ ortho_size_x }, ortho_size_y{ ortho_size_y }
		{}
		bool overlaps_with_box(const glm::vec2& ortho_pos, const glm::vec2& ortho_size) const {
			float x_dist = ortho_pos.x - ortho_x; if (x_dist < 0.0f) x_dist = -x_dist;
			float y_dist = ortho_pos.y - ortho_y; if (y_dist < 0.0f) y_dist = -y_dist;
			return
				x_dist <= (ortho_size.x + ortho_size_x) * 0.5f &&
				y_dist <= (ortho_size.y + ortho_size_y) * 0.5f;
		}
		bool overlaps_with_mouse(const float mouse_ortho_x, const float mouse_ortho_y) const {
			const float x_left = ortho_x - ortho_size_x * 0.5f;
			const float y_bottom = ortho_y - ortho_size_y * 0.5f;
			return
				mouse_ortho_x > x_left && mouse_ortho_x < x_left + ortho_size_x &&
				mouse_ortho_y > y_bottom && mouse_ortho_y < y_bottom + ortho_size_y;
		}
		uint32_t x = 0, y = 0, w = 0, h = 0;
		float ortho_x = 0.0f, ortho_y = 0.0f, ortho_size_x = 0.0f, ortho_size_y = 0.0f;
	};

	struct UI_RenderEntry {
		UI_RenderEntry() {}
		UI_RenderEntry(
			UI_Render_Type render_type, uint32_t index_count,
			uint16_t clip_rect_id, uint16_t content_offset_id,
			bool change_clip_rect, bool change_content_offset
		) :
			render_type{ render_type }, index_count{ index_count },
			clip_rect_id{ clip_rect_id }, content_offset_id{ content_offset_id },
			change_clip_rect{ change_clip_rect }, change_content_offset{ change_content_offset }
		{}
		uint32_t index_count = 0; //for render count
		uint16_t clip_rect_id = 0;
		uint16_t content_offset_id = 0;
		UI_Render_Type render_type = UI_Render_Type::UI_NONE; //for shader
		bool change_clip_rect = false;
		bool change_content_offset = false;
	};

	struct UI_HitEntry {
		UI_HitEntry() {}
		UI_HitEntry(void* component, uint16_t content_offset_id) :
			component{ component }, content_offset_id{ content_offset_id } {}
		void* component = nullptr;
		uint16_t content_offset_id = 0;
	};

	struct UI_RenderContext {
		std::vector<UI_RenderEntry>& render_queue;
		std::vector<UI_Vertex2f>& sprites_buffer;
		std::vector<UI_Text_Vertex2f>& sdf_text_buffer;
		std::vector<UI_HitEntry>& hit_queue;
		std::vector<ClipRectangle>& clip_rects;
		std::vector<glm::vec2>& content_offsets;
	};

	struct UI_RenderState {
		uint16_t clip_rect_id = 0;
		uint16_t content_offset_id = 0;
	};
}