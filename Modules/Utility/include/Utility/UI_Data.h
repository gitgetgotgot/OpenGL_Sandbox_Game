#pragma once
#include <glm/ext/matrix_float4x4.hpp>

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
		ClipRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h) :
			x{ x }, y{ y }, w{ w }, h{ h } {}
		uint32_t x = 0, y = 0, w = 0, h = 0;
	};

	struct UI_RenderEntry {
		UI_RenderEntry() {}
		UI_RenderEntry(
			UI_Render_Type render_type, uint32_t index_count,
			uint16_t clip_rect_id, bool change_clip_rect,
			float content_x_offset, float content_y_offset) :
			render_type{ render_type }, index_count{ index_count }, clip_rect_id{ clip_rect_id },
			change_clip_rect{ change_clip_rect }
		{}
		uint32_t index_count = 0; //for render count
		uint16_t clip_rect_id = 0;
		uint16_t content_offset_id = 0;
		UI_Render_Type render_type = UI_Render_Type::UI_NONE; //for shader
		bool change_clip_rect = false;
		bool change_content_offset = false;
	};

	struct UI_RenderContext {
		UI_RenderContext(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
			std::vector<UI_Object*>& hit_queue
		) : render_queue{ render_queue }, sprites_buffer{ sprites_buffer },
			sdf_text_buffer{ sdf_text_buffer }, hit_queue{ hit_queue }
		{}
		std::vector<UI_RenderEntry>& render_queue;
		std::vector<UI_Vertex2f>& sprites_buffer;
		std::vector<UI_Text_Vertex2f>& sdf_text_buffer;
		std::vector<UI_Object*>& hit_queue;
		std::vector<ClipRectangle>& clip_rects;
		std::vector<glm::vec2>& content_offsets;
	};

	struct UI_RenderState {
		uint16_t clip_rect_id = 0;
		uint16_t content_offset_id = 0;
	};
}