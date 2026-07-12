#version 460 core
out vec4 fragColor;

in vec2 texCoords;
in vec4 color;

layout (binding = 1) uniform sampler2D tex;

void main()
{
	float sd = texture(tex, texCoords).r;
	float edge = fwidth(sd);

	// глиф
	float fill = smoothstep(0.5 - edge, 0.5 + edge, sd);
	
	// обводка (мягкая)
	float outline = smoothstep(0.4 - edge, 0.4 + edge, sd) - smoothstep(0.5 - edge, 0.5 + edge, sd);
	
	// цвет
	vec3 final_color = mix(vec3(0.0), color.rgb, fill);
	
	// альфа
	float alpha = max(fill, outline);
	
	fragColor = vec4(final_color, alpha);
}