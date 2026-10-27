#version 460 core

out vec4 v_FragColor;

in vec2 v_TexUV;
in vec4 v_TextColor;

layout (binding = 10) uniform sampler2DArray u_textures;

void main() {
    float dist = texture(u_textures, vec3(v_TexUV, 0.0)).r;
    
    // automatic antialiasing
    float afWidth = fwidth(dist);
    
    // text border (in atlases sd is normalized from 0 to 1)
    float threshold = 0.5;
    
    // smooth edges of main text
    float alpha = smoothstep(threshold - afWidth, threshold + afWidth, dist);
    
    // smooth edges of outline
    float outlineWidth = 0.25;
    float outlineAlpha = smoothstep(threshold - outlineWidth - afWidth, threshold - outlineWidth + afWidth, dist);
    
    // final color: mix colors of outline and text based on distance
    vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 finalColor = mix(outlineColor, v_TextColor, alpha);
    
    // transparency is used as outer border of outline
    v_FragColor = vec4(finalColor.rgb, finalColor.a * outlineAlpha);
}