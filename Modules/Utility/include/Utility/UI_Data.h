#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

namespace CoreUI {
	struct UI_Vertex2f {
		UI_Vertex2f() {}
		UI_Vertex2f(float pX, float pY, float UVx, float UVy, uint32_t tex_index) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
			this->tex_index = tex_index;
		}
		UI_Vertex2f(float pX, float pY, float UVx, float UVy, uint32_t tex_index, glm::vec4 color) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
			this->color = color;
			this->tex_index = tex_index;
		}
		glm::vec2 pos{ 0.0f };
		glm::vec2 UV{ 0.0f };
		glm::vec4 color{ 1.0f };
		uint32_t tex_index = 0;
	};

	struct UI_Text_Vertex2f {
		UI_Text_Vertex2f() {}
		UI_Text_Vertex2f(float pX, float pY, float UVx, float UVy) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
		}
		UI_Text_Vertex2f(float pX, float pY, float UVx, float UVy, glm::vec4 color) {
			pos.x = pX; pos.y = pY;
			UV.x = UVx; UV.y = UVy;
			this->color = color;
		}
		glm::vec2 pos{ 0.0f };
		glm::vec2 UV{ 0.0f };
		glm::vec4 color{ 1.0f };
	};

	struct UI_UBO {
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};

	enum UI_Component_Type : uint8_t {
		UI_QUEUE_START, UI_EMPTY, UI_SPRITE, UI_TEXT
	};

	struct UI_RenderEntry {
		UI_RenderEntry() {}
		UI_RenderEntry(UI_Component_Type type, uint32_t index_count) : type{ type }, index_count{ index_count } {}
		UI_Component_Type type = UI_Component_Type::UI_QUEUE_START; //for shader
		uint32_t index_count = 0; //for render count
	};
}