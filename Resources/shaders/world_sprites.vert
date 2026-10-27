#version 460 core
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexUV;
layout (location = 2) in uint a_TexArrayID;
layout (location = 3) in uint a_TexID;

layout(std140, binding = 0) uniform UBO {
	mat4 u_ViewMatrix;
	mat4 u_ProjectionMatrix;
	ivec2 u_WorldSize;
	float u_DayRatio;
};

out vec2 v_GlobalPos;
out vec2 v_TexUV;
flat out uint v_TexArrayID;
flat out float v_TexID;

void main() {
	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(a_Pos, 0.0, 1.0);

	v_GlobalPos = a_Pos;
	v_TexUV = a_TexUV;
	v_TexArrayID = a_TexArrayID;
	v_TexID = float(a_TexID);
}