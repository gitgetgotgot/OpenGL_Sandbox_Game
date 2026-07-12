#version 460 core
layout (location = 0) in vec2 aPos;

struct Data {
	mat4 model;
	vec2 texCoords[4];
};

layout(std430, binding = 0) buffer EntitySSBO {
    Data objects[];
};

layout(std140, binding = 0) uniform UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	ivec2 world_size;
	float day_ratio;
};

out vec2 texCoord;
out vec2 globalCoord;

out float dayRatio;
flat out ivec2 worldSize;

void main() {
	mat4 modelMatrix = objects[gl_InstanceID].model;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 0.0, 1.0);

	texCoord = objects[gl_InstanceID].texCoords[gl_VertexID % 4];
	globalCoord = (modelMatrix * vec4(aPos, 0.0, 1.0)).xy;

	dayRatio = day_ratio;
	worldSize = world_size;
}