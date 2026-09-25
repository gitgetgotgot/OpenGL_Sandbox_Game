#pragma once
#include <Utility/UI_Data.h>
#include <vector>

namespace CoreUI {
	namespace CompFn {
		using Fn_Update_UI_Data = void(*)(void*, UI_RenderContext&, UI_RenderState&);
		using Fn_Void_Void = void(*)(void*);
	}

	class UI_Component_VTable {
	public:
		CompFn::Fn_Update_UI_Data update_buffer_data = nullptr;
		CompFn::Fn_Void_Void update_data = nullptr;
		CompFn::Fn_Void_Void destroy = nullptr;
	};

	class UI_ComponentBase {
	public:
		UI_ComponentBase() = default;
		UI_ComponentBase(uint32_t object_id, UI_Component_Type type, UI_Render_Type render_type) :
			object_id{ object_id }, type{ type }, render_type{ render_type } {}
		void update_render_queue(UI_RenderContext& c, UI_RenderState& s, uint32_t index_count);

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
				[] {
					if constexpr (requires(Derived& d, UI_RenderContext& c, UI_RenderState& s) { d._update_buffer_data(c, s); })
						return +[](void* p, UI_RenderContext& c, UI_RenderState& s) { static_cast<Derived*>(p)->_update_buffer_data(c, s); };
					else
						return nullptr;
				}(),

				[] {
					if constexpr (requires(Derived& d) { d._update_data(); })
						return +[](void* p) { static_cast<Derived*>(p)->_update_data(); };
					else
						return nullptr;
				}(),

				[](void* p) { delete static_cast<Derived*>(p); }
			};
			return &vt;
		}
	};
}