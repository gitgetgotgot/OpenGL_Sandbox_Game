#pragma once
#include "UI/UI_Object.h"

namespace CoreUI {
	class Canvas {
	public:
		void add_object(UI_Obj_Ptr& new_object);
		void update_canvas_objects_data(
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
			std::vector<UI_Object*>& hit_queue
		);
		void update_child_object(
			UI_Object& object,
			std::vector<UI_RenderEntry>& render_queue,
			std::vector<UI_Vertex2f>& sprites_buffer,
			std::vector<UI_Text_Vertex2f>& sdf_text_buffer,
			std::vector<UI_Object*>& hit_queue,
			uint16_t clip_rect_id
		);
		uint16_t add_clip_rect(UI_Transform& tr);
		UI_Transform transform;
		// Canvas stores UI objects in ordered container
		std::vector<uint32_t> objects;
		bool is_enabled = true;
	};

	class UI_Canvas_Ptr {
	public:
		UI_Canvas_Ptr() {}
		UI_Canvas_Ptr(uint32_t id) : id{ id } {}
		Canvas* operator->() const;
		Canvas& operator*() const;
		operator bool() const;
		UI_Canvas_Ptr& operator=(const UI_Canvas_Ptr&) noexcept;
		UI_Canvas_Ptr& operator=(UI_Canvas_Ptr&&) noexcept;
		uint32_t get_id() const { return id; }
		void reset() { id = 0; }
	private:
		uint32_t id = 0;
	};
}