#pragma once

#include <glm/glm.hpp>

namespace GameMath {
	inline bool mouse_overlaps_ortho_square(
		glm::vec2& square_ortho_pos, glm::vec2& square_ortho_size,
		float mouse_ortho_x, float mouse_ortho_y
	) {
		float left_x = square_ortho_pos.x - square_ortho_size.x * 0.5f;
		float bottom_y = square_ortho_pos.y - square_ortho_size.y * 0.5f;
		return
			mouse_ortho_x > left_x &&
			mouse_ortho_x < left_x + square_ortho_size.x &&
			mouse_ortho_y > bottom_y &&
			mouse_ortho_y < bottom_y + square_ortho_size.y;
	}
	//translation with pivot (0, 0)
	inline glm::mat4 build_2D_TRS(const glm::vec2& center, const glm::vec2& size, const float& rotation_angle) {
		glm::mat4 mat(1.f);

		float c = cos(rotation_angle);
		float s = sin(rotation_angle);

		mat[0][0] = c * size.x; //rotate + scale
		mat[0][1] = s * size.x;
		mat[1][0] = -s * size.y;
		mat[1][1] = c * size.y;

		// pivot correction with rotation
		float dx = -0.5f * c + 0.5f * s;
		float dy = -0.5f * s - 0.5f * c;

		mat[3][0] = center.x + dx * size.x; //translate to center with correction
		mat[3][1] = center.y + dy * size.y;

		return mat;
	}
	//translation with pivot (0, 0)
	inline void update_2D_TRS(glm::mat4& mat, const glm::vec2& center, const glm::vec2& size, const float& rotation_angle) {
		mat = glm::mat4(1.f);
		
		float c = cos(rotation_angle);
		float s = sin(rotation_angle);

		mat[0][0] = c * size.x; //rotate + scale
		mat[0][1] = s * size.x;
		mat[1][0] = -s * size.y;
		mat[1][1] = c * size.y;

		// pivot correction with rotation
		float dx = -0.5f * c + 0.5f * s;
		float dy = -0.5f * s - 0.5f * c;

		mat[3][0] = center.x + dx * size.x; //translate to center with correction
		mat[3][1] = center.y + dy * size.y;
	}

	//translation with pivot (0.5, 0.5)
	inline glm::mat4 build_2D_TS(const glm::vec2& center, const glm::vec2& scale) {
		glm::mat4 mat(1.f);

		mat[0][0] = scale.x; //scale
		mat[1][1] = scale.y;

		mat[3][0] = center.x; //translate
		mat[3][1] = center.y;

		return mat;
	}
	//translation with pivot (0.5, 0.5)
	inline void update_2D_TS(glm::mat4& mat, const glm::vec2& center, const glm::vec2& scale) {
		mat = glm::mat4(1.f);

		mat[0][0] = scale.x; //scale
		mat[1][1] = scale.y;

		mat[3][0] = center.x; //translate
		mat[3][1] = center.y;
	}
}