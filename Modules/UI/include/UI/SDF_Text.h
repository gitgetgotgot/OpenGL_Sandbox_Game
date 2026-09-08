#pragma once
#include "UI/UI_Component.h"
#include <string>

namespace CoreUI {
	class SDF_Text : public UI_Component {
	public:
		SDF_Text(uint32_t object_id) : UI_Component(object_id) {}
		virtual void _update_sdf_text_buffer(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer
		) override;
		virtual void _update_render_data() override;
		std::string text;
	private:
		std::vector<UI_Text_Vertex2f> text_vertices;
	};
}