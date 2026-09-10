#pragma once
#include <Utility/UI_Data.h>

namespace CoreUI {
	class UI_Component {
	public:
		UI_Component(uint32_t object_id, UI_Component_Type type, UI_Render_Type render_type) :
			object_id{ object_id }, type{ type }, render_type{ render_type } {}
		virtual ~UI_Component() {}
		uint32_t object_id = 0;
		UI_Component_Type type = UI_Component_Type::UI_WIDGET;
		UI_Render_Type render_type = UI_Render_Type::UI_NONE;
		bool is_visible = true;
		bool is_interactable = false;
		virtual void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			uint16_t clip_rect_id
		) {}
		virtual void _update_sdf_text_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
			uint16_t clip_rect_id
		) {}
		virtual void _update_render_data() = 0;
		void update_render_queue(std::vector<UI_RenderEntry>& render_queue, uint16_t clip_rect_id, uint32_t index_count);
	protected:
		bool is_dirty = false;
		void mark_dirty();
	};
}	