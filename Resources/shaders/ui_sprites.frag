#version 460 core

out vec4 fragColor;

in vec2 texCoord;
flat in uint texID;
in vec4 Color;

layout (binding = 0) uniform sampler2DArray u_tex;

void main()
{
    vec4 texColor = texture(u_tex, vec3(texCoord, texID));
    if(texColor.a == 0.0) discard;
    fragColor = texColor * Color;
}