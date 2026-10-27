#version 460 core

out vec4 v_FragColor;

in vec2 v_TexUV;
in vec4 v_Color;
flat in uint v_TexArrayID;
flat in float v_TexID;

layout (binding = 0) uniform sampler2DArray u_textures[8];

void main() {
    vec2 tiled_UV = fract(v_TexUV);
    vec4 texColor = texture(u_textures[v_TexArrayID], vec3(tiled_UV, v_TexID));
    v_FragColor = texColor * v_Color;
}