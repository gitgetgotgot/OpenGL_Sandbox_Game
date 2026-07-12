#version 460 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 Color;

layout (binding = 0) uniform sampler2D u_tex;

void main()
{
    vec4 texColor = texture(u_tex, texCoord);
    if(texColor.a == 0.0) discard;
    fragColor = texColor * Color;
}