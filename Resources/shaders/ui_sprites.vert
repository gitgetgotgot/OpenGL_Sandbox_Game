#version 460 core

layout (location = 0) in vec2 _Pos;
layout (location = 1) in vec2 _UV;
layout (location = 2) in vec4 _Color;
layout (location = 3) in uint _TexArrayID;
layout (location = 4) in uint _TexID;

layout(std140, binding = 1) uniform UI_UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec2 content_offset;
};

out vec2 texUV;
flat out uint texArrayID;
flat out uint texID;
out vec4 Color;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(_Pos.x + content_offset.x, _Pos.y + content_offset.y, 0.0, 1.0);
	texUV = _UV;
	texArrayID = _TexArrayID;
	texID = _TexID;
	Color = _Color;
}