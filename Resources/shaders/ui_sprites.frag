#version 460 core

out vec4 fragColor;

in vec2 texUV;
flat in uint texArrayID;
flat in uint texID;
in vec4 Color;

layout (binding = 0) uniform sampler2DArray u_textures[8];

void main()
{
    vec4 texColor = texture(u_textures[texArrayID], vec3(texUV, float(texID)));
    fragColor = texColor * Color;
}