#version 460 core

layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexUV;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in uint a_TexArrayID;
layout (location = 4) in uint a_TexID;

layout(std140, binding = 1) uniform UI_UBO {
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	vec2 u_ContentOffset;
};

out vec2 v_TexUV;
out vec4 v_Color;
flat out uint v_TexArrayID;
flat out float v_TexID;

void main() {
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(a_Pos.x + u_ContentOffset.x, a_Pos.y + u_ContentOffset.y, 0.0, 1.0);
	v_TexUV = a_TexUV;
	v_Color = a_Color;
	v_TexArrayID = a_TexArrayID;
	v_TexID = float(a_TexID);
}