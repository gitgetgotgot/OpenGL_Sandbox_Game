#pragma once
#include <vector>
#include <glm/ext/vector_float2.hpp>

namespace CoreUI {
	class UI_Transform {
		friend class UI_ObjectManager;
	public:
		UI_Transform() {}
		void set_local_pos(float x, float y);
		void set_local_pos(glm::vec2 pos);
		void set_size(float width, float height);
		void set_size(glm::vec2 size);
		void add_child(UI_Transform& transform);
		void translate(float x, float y);
		void translate(glm::vec2 vec);
		glm::vec2 global_pos{ 0.0f };
		glm::vec2 local_pos{ 0.0f };
		glm::vec2 size{ 1.0f };
		std::vector<uint32_t> children;
		uint32_t parent_id = 0; //0 means empty
		uint32_t object_id = 0;
	private:
		inline static uint8_t COMPONENT_TRANSFORM = 1 << 0;
		bool is_dirty = false;
		void update_transform();
		void mark_dirty();
	};
}