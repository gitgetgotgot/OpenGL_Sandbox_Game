#pragma once
#include <cstdint>
#include <Utility/UI_Data.h>

namespace CoreUI {
	class UI_Component {
	public:
		UI_Component(uint32_t object_id) : object_id{ object_id } {}
		virtual ~UI_Component() {}
		uint32_t object_id = 0;
		UI_Component_Type type = UI_Component_Type::UI_EMPTY;
		bool is_visible = true;
		virtual void _update_sprite_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer
		) {}
		virtual void _update_sdf_text_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer
		) {}
		virtual void _update_render_data() = 0;
	protected:
		bool is_dirty = false;
		void mark_dirty();
	};
}