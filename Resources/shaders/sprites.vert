#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

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

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 0.0, 1.0);
	texCoord = aTex;
	globalCoord = aPos;

	dayRatio = day_ratio;
	worldSize = world_size;
}