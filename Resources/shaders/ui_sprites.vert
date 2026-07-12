#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec4 aColor;

layout(std140, binding = 1) uniform UI_UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
};

out vec2 texCoord;
out vec4 Color;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 0.0, 1.0);
	texCoord = aTex;
	Color = aColor;
}