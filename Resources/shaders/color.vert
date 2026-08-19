#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

layout(std140, binding = 0) uniform UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	ivec2 world_size;
	float day_ratio;
};

out vec4 color;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 0.0, 1.0);
	color = aColor;
}