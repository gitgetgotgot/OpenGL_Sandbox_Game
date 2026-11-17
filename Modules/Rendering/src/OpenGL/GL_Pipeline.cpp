#include "Rendering/OpenGL/GL_Pipeline.h"
#include "Rendering/OpenGL/GL_Shader.h"
#include "Rendering/OpenGL/GL_VL.h"
#include "Rendering/OpenGL/GL_UB.h"
#include "Rendering/OpenGL/GL_SB.h"
#include "Rendering/OpenGL/GL_Texture.h"
#include "Rendering/OpenGL/GL_TextureArray.h"

Engine::Graphics::GL_Pipeline::~GL_Pipeline() {
	uninit();
}

void Engine::Graphics::GL_Pipeline::init(
	const ShaderHandle& shader,
	const VertexLayout& layout,
	PipelinePrimitives p,
	PipelineScissor s,
	PipelineViewport v,
	PipelineBlendMode bm,
	PipelineDepthMode dm,
	PipelineCullMode cm,
	std::error_code& ec
) {
	this->shader = &static_cast<const GL_Shader&>(shader);
	this->vl = &static_cast<const GL_VL&>(layout);
}

void Engine::Graphics::GL_Pipeline::uninit() {
	shader = nullptr;
	vl = nullptr;
}

void Engine::Graphics::GL_Pipeline::bind_uniform_buffer(const UniformBuffer& ub, uint32_t binding, uint32_t set) {
    const auto& gl_ub = static_cast<const GL_UB&>(ub);
	gl_ub.bind(binding);
}

void Engine::Graphics::GL_Pipeline::bind_storage_buffer(const StorageBuffer& sb, uint32_t binding, uint32_t set) {
    const auto& gl_sb = static_cast<const GL_SB&>(sb);
	gl_sb.bind(binding);
}

void Engine::Graphics::GL_Pipeline::bind_sampler_2D(const Texture& tex, uint32_t binding, uint32_t set) {
	const auto& gl_tex = static_cast<const GL_Texture&>(tex);
	gl_tex.bind(binding);
}

void Engine::Graphics::GL_Pipeline::bind_sampler_2D_array(const TextureArray& tex_arr, uint32_t binding, uint32_t index, uint32_t set) {
    const auto& gl_tex_arr = static_cast<const GL_TextureArray&>(tex_arr);
	// for OpenGL global samplers array binding = [0, 7], so for slot just use index = [0, 7]
	gl_tex_arr.bind(index);
}


void Engine::Graphics::GL_Pipeline::bind() const {
	const GL_Shader* gl_shader = static_cast<const GL_Shader*>(shader);
	const GL_VL* gl_vl = static_cast<const GL_VL*>(vl);
	gl_shader->use_shader();
	gl_vl->bind();
}