#version 460 core
out vec4 fragColor;

in vec2 texCoords;
in vec4 color;

layout (binding = 10) uniform sampler2DArray tex;

void main() {
	// Читаем значение SDF из текстуры (хранится в красном канале)
    float dist = texture(tex, vec3(texCoords, 0.0)).r;
    
    // Автоматический антиалиасинг на основе пиксельных производных
    float afWidth = fwidth(dist);
    
    // Порог границы текста (в нашем генераторе это 0.5, так как sd нормализован от 0 до 1)
    float threshold = 0.5;
    
    // Сглаживание краев основного текста
    float alpha = smoothstep(threshold - afWidth, threshold + afWidth, dist);
    
    // Сглаживание краев обводки (смещаем порог внутрь на outlineWidth)
    float outlineWidth = 0.25;
    float outlineAlpha = smoothstep(threshold - outlineWidth - afWidth, threshold - outlineWidth + afWidth, dist);
    
    // Итоговый цвет: смешиваем цвет обводки и текста в зависимости от расстояния
    vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 finalColor = mix(outlineColor, color, alpha);
    
    // Прозрачность задается внешней границей обводки
    fragColor = vec4(finalColor.rgb, finalColor.a * outlineAlpha);
}