#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec4 aColor;
layout (location = 3) in uint aTexID;

layout(std140, binding = 1) uniform UI_UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec2 content_offset;
};

out vec2 texCoord;
flat out uint texID;
out vec4 Color;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(aPos.x + content_offset.x, aPos.y + content_offset.y, 0.0, 1.0);
	texCoord = aTex;
	texID = aTexID;
	Color = aColor;
}