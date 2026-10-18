#version 460 core

out vec4 fragColor;

in vec2 texUV;
flat in uint texArrayID;
flat in uint texID;
in vec4 Color;

layout (binding = 0) uniform sampler2DArray u_textures[8];

void main() {
    // fract can work not so good with mipmaps, but for UI with no mipmaps works perfectly
    vec2 tiled_UV = fract(texUV);
    vec4 texColor = texture(u_textures[texArrayID], vec3(tiled_UV, float(texID)));
    fragColor = texColor * Color;
}