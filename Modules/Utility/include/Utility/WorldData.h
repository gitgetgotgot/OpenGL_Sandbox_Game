#pragma once
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>

constexpr uint32_t CHUNK_SIZE = 64;

struct WorldSlot {
	uint16_t tile_id = 0;
	uint16_t wall_id = 0;
	uint16_t flags = 0;
};

struct Vertex2f {
	Vertex2f() {}
	Vertex2f(float pX, float pY, float UVx, float UVy, uint32_t tex_index) {
		pos.x = pX; pos.y = pY;
		UV.x = UVx; UV.y = UVy;
		texture_array_index = tex_index;
	}
	glm::vec2 pos{ 0.0f };
	glm::vec2 UV{ 0.0f };
	uint32_t texture_array_index = 0;
};
