#include "Rendering/OpenGL/GL_Pipeline.h"
#include "Rendering/OpenGL/GL_Shader.h"
#include "Rendering/OpenGL/GL_VL.h"

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

void Engine::Graphics::GL_Pipeline::bind() const {
	const GL_Shader* gl_shader = static_cast<const GL_Shader*>(shader);
	const GL_VL* gl_vl = static_cast<const GL_VL*>(vl);
	gl_shader->use_shader();
	gl_vl->bind();
}