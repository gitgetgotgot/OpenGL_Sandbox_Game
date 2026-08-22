#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in uint aTexID;

layout(std140, binding = 0) uniform UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	ivec2 world_size;
	float day_ratio;
};

out vec2 texCoord;
flat out uint texID;
out vec2 globalCoord;
out float dayRatio;
flat out ivec2 worldSize;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 0.0, 1.0);
	texCoord = aTex;
	texID = aTexID;
	globalCoord = aPos;

	dayRatio = day_ratio;
	worldSize = world_size;
}