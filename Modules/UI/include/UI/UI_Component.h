#pragma once
#include <Utility/UI_Data.h>
#include <vector>

namespace CoreUI {
	namespace CompFn {
		using Fn_Update_Sprite = void(*)(void*, std::vector<UI_RenderEntry>&, std::vector<UI_Vertex2f>&, uint16_t);
		using Fn_Update_Text = void(*)(void*, std::vector<UI_RenderEntry>&, std::vector<UI_Text_Vertex2f>&, uint16_t);
		using Fn_Void_Void = void(*)(void*);
	}

	class UI_Component_VTable {
	public:
		CompFn::Fn_Update_Sprite update_sprite_data = nullptr;
		CompFn::Fn_Update_Text update_text_data = nullptr;
		CompFn::Fn_Void_Void update_render_data = nullptr;
		CompFn::Fn_Void_Void destroy = nullptr;
	};

	class UI_ComponentBase {
	public:
		UI_ComponentBase() = default;
		UI_ComponentBase(uint32_t object_id, UI_Component_Type type, UI_Render_Type render_type) :
			object_id{ object_id }, type{ type }, render_type{ render_type } {}
		void update_render_queue(std::vector<UI_RenderEntry>& render_queue, uint16_t clip_rect_id, uint32_t index_count);

		uint32_t object_id = 0;
		UI_Component_Type type = UI_Component_Type::UI_WIDGET;
		UI_Render_Type render_type = UI_Render_Type::UI_NONE;
		bool is_visible = true;
		bool is_interactable = false;
		bool blocks_input = false;

		uint8_t dirty_mask = 255U;
	protected:
		bool is_dirty = false;
		void mark_dirty();
	};

	template<typename Derived>
	class UI_Component : public UI_ComponentBase {
	public:
		UI_Component() = default;
		UI_Component(uint32_t object_id, UI_Component_Type type, UI_Render_Type render_type) :
			UI_ComponentBase(object_id, type, render_type) {}

		static UI_Component_VTable* get_vt() {
			static UI_Component_VTable vt{
				[] { if constexpr (requires(Derived& d, std::vector<UI_RenderEntry>& re, std::vector<UI_Vertex2f>& sb, uint16_t clip_rect_id) {
					d._update_sprite_buffer(re, sb, clip_rect_id);
				})
					return +[](void* p, std::vector<UI_RenderEntry>& re, std::vector<UI_Vertex2f>& sb, uint16_t clip_rect_id)
						{ static_cast<Derived*>(p)->_update_sprite_buffer(re, sb, clip_rect_id); };
					else return nullptr;
				}(),

				[] { if constexpr (requires(Derived& d, std::vector<UI_RenderEntry>&re, std::vector<UI_Text_Vertex2f>&tb, uint16_t clip_rect_id) {
					d._update_sdf_text_buffer(re, tb, clip_rect_id);
				})
					return +[](void* p, std::vector<UI_RenderEntry>& re, std::vector<UI_Text_Vertex2f>& tb, uint16_t clip_rect_id)
						{ static_cast<Derived*>(p)->_update_sdf_text_buffer(re, tb, clip_rect_id); };
					else return nullptr;
				}(),

				[] { if constexpr (requires(Derived& d) {
					d._update_render_data();
				})
					return +[](void* p)
						{ static_cast<Derived*>(p)->_update_render_data();  }; else return nullptr;
				}(),

				[](void* p) { delete static_cast<Derived*>(p); }
			};
			return &vt;
		}
	};
}