#pragma once
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_precision.hpp>

constexpr uint32_t CHUNK_SIZE = 64;

struct Universal_UBO_Data {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::ivec2 world_size;
	float day_ratio;
	float padding;
};

struct ShaderLightingInfo {
	ShaderLightingInfo() {}
	ShaderLightingInfo(uint8_t r, uint8_t g, uint8_t b, uint8_t global_value, uint8_t source_value) {
		color.x = r; color.y = g; color.z = b;
		global_light_value = global_value;
		source_light_value = source_value;
	}
	glm::u8vec3 color;
	uint8_t global_light_value;
	uint8_t source_light_value;
};

struct SpriteData {
	glm::mat4 modelMatrix;
	glm::vec2 tex_coords[4];
	float light_level = 1.f, opacity = 1.f; //light level and opacity are from 0.0 to 1.0 (can be used for weather system)
	float padding[2]{};
};

struct WorldSlot {
	uint16_t tile_id = 0;
	uint16_t wall_id = 0;
	uint16_t flags = 0;
};

struct InventorySlot {
	uint16_t item_id = 0;
	uint16_t amount = 0;
};

struct EffectEntry {
	uint16_t id = 0;
	float duration = 0.0f;
};

struct DropEntry {
	uint16_t id = 0;
	uint16_t amount = 0;
};