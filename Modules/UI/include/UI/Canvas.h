#pragma once
#include "UI/UI_Object.h"
#include <memory>

namespace CoreUI {
	class Canvas {
	public:
		void add_object(UI_Object* new_object);
		void update_canvas_objects_data(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer
		);
		void update_child_object(
			UI_Object& object,
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer
		);
		UI_Transform transform;
		// Canvas stores UI objects in ordered container
		std::vector<uint32_t> objects;
		bool is_enabled = true;
	};
}