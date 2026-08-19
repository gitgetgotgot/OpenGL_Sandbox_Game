#version 460 core
layout (location = 0) in vec2 aPos;

struct Data {
	mat4 model;
	vec4 UV;
};

layout(std430, binding = 0) buffer EntitySSBO {
    Data objects[];
};

layout(std140, binding = 0) uniform UBO {
	mat4 viewMatrix;
	mat4 projectionMatrix;
	ivec2 world_size;
	float day_ratio;
};

out vec2 texCoord;
out vec2 globalCoord;

out float dayRatio;
flat out ivec2 worldSize;

vec2 get_quad_UV(int vertex_id){
	vec4 uv = objects[gl_InstanceID].UV;
	float u = uv.x + (vertex_id == 2 || vertex_id == 3 ? uv.z : 0.0);
    float v = uv.y + (vertex_id == 1 || vertex_id == 2 ? uv.w : 0.0);
    return vec2(u, v);
}

void main() {
	mat4 modelMatrix = objects[gl_InstanceID].model;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 0.0, 1.0);

	texCoord = get_quad_UV(int(gl_VertexID % 4));
	globalCoord = (modelMatrix * vec4(aPos, 0.0, 1.0)).xy;

	dayRatio = day_ratio;
	worldSize = world_size;
}