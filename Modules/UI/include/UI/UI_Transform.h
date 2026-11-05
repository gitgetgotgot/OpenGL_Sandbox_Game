#pragma once
#include <vector>
#include <glm/ext/vector_float2.hpp>

namespace Engine::UI {
	class UI_Transform {
		friend class UI_ObjectManager;
	public:
		enum Anchor : uint8_t {
			BottomLeft, BottomCenter, BottomRight,
			MiddleLeft, MiddleCenter, MiddleRight,
			TopLeft,	TopCenter,    TopRight
		};
		UI_Transform() {}
		void set_anchor(Anchor anchor);
		void set_local_pos(float x, float y);
		void set_local_pos(glm::vec2 pos);
		void set_size(float width, float height);
		void set_size(glm::vec2 size);
		void set_pivot(float pivot_x, float pivot_y);
		void set_pivot(glm::vec2 pivot);
		void set_width(float width);
		void set_height(float height);
		void add_child(UI_Transform& transform);
		bool remove_child(UI_Transform& transform);
		bool remove_child(uint32_t child_id);
		void translate(float x, float y);
		void translate(glm::vec2 vec);
		void force_update();
		glm::vec2 global_pos{ 0.0f };
		glm::vec2 local_pos{ 0.0f };
		glm::vec2 size{ 1.0f };
		glm::vec2 pivot{ 0.5f };
		std::vector<uint32_t> children;
		uint32_t parent_id = 0; //0 means empty
		uint32_t object_id = 0;
		Anchor anchor = Anchor::MiddleCenter;
	private:
		static constexpr glm::vec2 ANCHOR_MULTIPLIERS[] = {
			{-0.5f, -0.5f}, {0.0f, -0.5f}, {0.5f, -0.5f},
			{-0.5f, 0.0f},  {0.0f, 0.0f},  {0.5f, 0.0f},
			{-0.5f, 0.5f},  {0.0f, 0.5f},  {0.5f, 0.5f}
		};
		inline static uint8_t COMPONENT_TRANSFORM = 1 << 0;
		bool is_dirty = false;
		bool is_destroyed = false;
		void update_transform();
		void mark_dirty();
		void mark_destroyed();
	};
}