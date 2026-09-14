#pragma once
#include <cstdint>
#include <iostream>

namespace CoreUI {
	struct UI_Behaviour_VTable {
		void(*on_pointer_enter)(void*) = nullptr;
		void(*on_pointer_hover)(void*) = nullptr;
		void(*on_pointer_exit)(void*) = nullptr;
		void(*on_pointer_down)(void*) = nullptr;
		void(*on_pointer_held)(void*) = nullptr;
		void(*on_pointer_up)(void*) = nullptr;
		void(*destroy)(void*) = nullptr;
	};

	class UI_BehaviourBase {
	public:
		bool is_pressed = false;
		bool is_pointed = false;
	};

	template<typename Derived>
	class UI_Behaviour : public UI_BehaviourBase {
	public:
		static UI_Behaviour_VTable* get_vt() {
			static UI_Behaviour_VTable vt{
				[] { if constexpr (requires(Derived d) { d.OnPointerEnter(); }) return +[](void* p) { static_cast<Derived*>(p)->OnPointerEnter(); }; else return nullptr; }(),
				[] { if constexpr (requires(Derived d) { d.OnPointerHover(); }) return +[](void* p) { static_cast<Derived*>(p)->OnPointerHover(); }; else return nullptr; }(),
				[] { if constexpr (requires(Derived d) { d.OnPointerExit();  }) return +[](void* p) { static_cast<Derived*>(p)->OnPointerExit();  }; else return nullptr; }(),
				[] { if constexpr (requires(Derived d) { d.OnPointerDown();  }) return +[](void* p) { static_cast<Derived*>(p)->OnPointerDown();  }; else return nullptr; }(),
				[] { if constexpr (requires(Derived d) { d.OnPointerHeld();  }) return +[](void* p) { static_cast<Derived*>(p)->OnPointerHeld();  }; else return nullptr; }(),
				[] { if constexpr (requires(Derived d) { d.OnPointerUp();    }) return +[](void* p) { static_cast<Derived*>(p)->OnPointerUp();    }; else return nullptr; }(),
				[](void* p) { static_cast<Derived*>(p)->~Derived(); }
			};
			return &vt;
		}
	};
}