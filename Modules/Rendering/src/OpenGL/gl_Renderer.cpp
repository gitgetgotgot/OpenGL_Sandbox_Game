#include "Rendering/OpenGL/GL_Renderer.h"
#include "Rendering/OpenGL/GL_Shader.h"
#include "Rendering/OpenGL/GL_VL.h"
#include "Rendering/OpenGL/GL_VB.h"
#include "Rendering/OpenGL/GL_IB.h"
#include "Rendering/OpenGL/GL_UB.h"
#include "Rendering/OpenGL/GL_SB.h"
#include "Rendering/OpenGL/GL_Texture.h"
#include "Rendering/OpenGL/GL_TextureArray.h"
#include <glad/gl.h>
#include <Utility/Logger.h>

Engine::Graphics::OpenGL_Renderer::OpenGL_Renderer(std::error_code& ec) {
	//openGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// read texture bytes without alignment or padding (correctly load textures of different formats and sizes)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void Engine::Graphics::OpenGL_Renderer::start_frame(std::error_code& ec) {
	// no need to prepare frame for rendering in OpenGL
}

void Engine::Graphics::OpenGL_Renderer::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::Graphics::OpenGL_Renderer::present_frame(std::error_code& ec) {
	// OpenGL swap buffers is controlled by GLFW
}

void Engine::Graphics::OpenGL_Renderer::on_window_resize(uint32_t window_width, uint32_t window_height, PresentMode mode) {
	glViewport(0, 0, window_width, window_height);
}

void Engine::Graphics::OpenGL_Renderer::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	glViewport(x, y, width, height);
}

void Engine::Graphics::OpenGL_Renderer::use_depth_test(bool use) {
	if (use) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void Engine::Graphics::OpenGL_Renderer::use_depth_write(bool use) {
	glDepthMask(use);
}

void Engine::Graphics::OpenGL_Renderer::use_depth_func(DepthFunc func) {
	GLenum fun{};
	glDepthFunc(fun);
}

void Engine::Graphics::OpenGL_Renderer::use_scissor_test(bool use) {
	if (use) glEnable(GL_SCISSOR_TEST);
	else glDisable(GL_SCISSOR_TEST);
}

void Engine::Graphics::OpenGL_Renderer::set_scissor_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	glScissor(x, y, width, height);
}

void Engine::Graphics::OpenGL_Renderer::bind_pipeline(const Pipeline& p) {
	// blank, OpenGL doesn't need to use prepared pipelines to use shaders and vertex layouts
}


std::unique_ptr<Engine::Graphics::ShaderHandle> Engine::Graphics::OpenGL_Renderer::create_ShaderPr(
	const std::filesystem::path& vertex_shader_path,
	const std::filesystem::path& fragment_shader_path,
	std::error_code& ec
) const {
	return std::make_unique<GL_Shader>(vertex_shader_path, fragment_shader_path);
}

std::unique_ptr<Engine::Graphics::VertexLayout> Engine::Graphics::OpenGL_Renderer::create_VertexLayout(std::error_code& ec) const {
	return std::make_unique<GL_VL>();
}

std::unique_ptr<Engine::Graphics::VertexBuffer> Engine::Graphics::OpenGL_Renderer::create_VertexBuffer(std::error_code& ec) const {
	return std::make_unique<GL_VB>();
}

std::unique_ptr<Engine::Graphics::IndexBuffer> Engine::Graphics::OpenGL_Renderer::create_IndexBuffer(std::error_code& ec) const {
	return std::make_unique<GL_IB>();
}

std::unique_ptr<Engine::Graphics::UniformBuffer> Engine::Graphics::OpenGL_Renderer::create_UniformBuffer(std::error_code& ec) const {
	return std::make_unique<GL_UB>();
}

std::unique_ptr<Engine::Graphics::StorageBuffer> Engine::Graphics::OpenGL_Renderer::create_StorageBuffer(std::error_code& ec) const {
	return std::make_unique<GL_SB>();
}

std::unique_ptr<Engine::Graphics::Texture> Engine::Graphics::OpenGL_Renderer::create_Texture(std::error_code& ec) const {
	return std::make_unique<GL_Texture>();
}

std::unique_ptr<Engine::Graphics::TextureArray> Engine::Graphics::OpenGL_Renderer::create_TextureArray(std::error_code& ec) const {
	return std::make_unique<GL_TextureArray>();
}


void Engine::Graphics::OpenGL_Renderer::renderIndexedData(
	const ShaderHandle& shader,
	const VertexLayout& vl,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t baseIndex = 0,
	const UniformBuffer* ub = nullptr,
	const StorageBuffer* sb = nullptr
) {
	const auto& gl_shader = static_cast<const GL_Shader&>(shader);
	const auto& gl_vl = static_cast<const GL_VL&>(vl);
	gl_shader.use_shader();
	gl_vl.bind();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(baseIndex * sizeof(uint32_t)));
}

void Engine::Graphics::OpenGL_Renderer::renderIndexedDataVAOSpecific(
	const VertexLayout& va,
	uint32_t indexCount
) {
	va.bind();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void Engine::Graphics::OpenGL_Renderer::renderInstancedData(
	const ShaderHandle& shader,
	const VertexLayout& vl,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t instanceCount
) {
	const auto& gl_shader = static_cast<const GL_Shader&>(shader);
	const auto& gl_vl = static_cast<const GL_VL&>(vl);
	gl_shader.use_shader();
	gl_vl.bind();
	glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
}

void Engine::Graphics::OpenGL_Renderer::renderBasedInstancedData(
	const ShaderHandle& shader,
	const VertexLayout& va,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t instanceCount,
	uint32_t baseInstance
) {
	shader.use_shader();
	va.bind();
	glDrawArraysInstancedBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, instanceCount, baseInstance);
}