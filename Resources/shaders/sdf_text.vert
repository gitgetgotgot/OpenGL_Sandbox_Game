#version 460 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec4 textColor;

layout(std140, binding = 1) uniform UI_UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec2 content_offset;
};

out vec2 texCoords;
out vec4 color;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(position.x + content_offset.x, position.y + content_offset.y, 0.0, 1.0);
	texCoords = tex;
	color = textColor;
}