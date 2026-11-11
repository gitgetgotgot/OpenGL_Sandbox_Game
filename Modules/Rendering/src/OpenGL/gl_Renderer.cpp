#include "Rendering/OpenGL/gl_Renderer.h"
#include "Rendering/OpenGL/gl_ShaderPr.h"
#include "Rendering/OpenGL/gl_VL.h"
#include "Rendering/OpenGL/gl_VB.h"
#include "Rendering/OpenGL/gl_IB.h"
#include "Rendering/OpenGL/gl_UB.h"
#include "Rendering/OpenGL/gl_SB.h"
#include "Rendering/OpenGL/gl_Texture.h"
#include "Rendering/OpenGL/gl_TextureArray.h"
#include <glad/gl.h>
#include <glm/ext/vector_float2.hpp>
#include <Utility/Logger.h>

Engine::Graphics::OpenGL_Renderer::OpenGL_Renderer(std::error_code& ec) {
	//openGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// read texture bytes without alignment or padding (correctly load textures of different formats and sizes)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void Engine::Graphics::OpenGL_Renderer::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::Graphics::OpenGL_Renderer::end_frame() {
	// empty here, but can be used for Vulkan, DirectX
}

void Engine::Graphics::OpenGL_Renderer::setViewport(float x, float y, float width, float height) {
	glViewport(x, y, width, height);
}

void Engine::Graphics::OpenGL_Renderer::useDepthTest(bool use) {
	if (use) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void Engine::Graphics::OpenGL_Renderer::useDepthWrite(bool use) {
	glDepthMask(use);
}

void Engine::Graphics::OpenGL_Renderer::useDepthFunc(DepthFunc func) {
	GLenum fun{};
	glDepthFunc(fun);
}

void Engine::Graphics::OpenGL_Renderer::useScissorTest(bool use) {
	if (use) glEnable(GL_SCISSOR_TEST);
	else glDisable(GL_SCISSOR_TEST);
}

void Engine::Graphics::OpenGL_Renderer::setScissorRect(float x, float y, float width, float height) {
	glScissor(x, y, width, height);
}

std::unique_ptr<Engine::Graphics::ShaderProgram> Engine::Graphics::OpenGL_Renderer::create_ShaderPr(
	const std::filesystem::path& vertex_shader_path,
	const std::filesystem::path& fragment_shader_path,
	std::error_code& ec
) const {
	return std::make_unique<gl_ShaderPr>(vertex_shader_path, fragment_shader_path);
}

std::unique_ptr<Engine::Graphics::VertexLayout> Engine::Graphics::OpenGL_Renderer::create_VertexLayout(std::error_code& ec) const {
	return std::make_unique<gl_VL>();
}

std::unique_ptr<Engine::Graphics::VertexBuffer> Engine::Graphics::OpenGL_Renderer::create_VertexBuffer(std::error_code& ec) const {
	return std::make_unique<gl_VB>();
}

std::unique_ptr<Engine::Graphics::IndexBuffer> Engine::Graphics::OpenGL_Renderer::create_IndexBuffer(std::error_code& ec) const {
	return std::make_unique<gl_IB>();
}

std::unique_ptr<Engine::Graphics::UniformBuffer> Engine::Graphics::OpenGL_Renderer::create_UniformBuffer(std::error_code& ec) const {
	return std::make_unique<gl_UB>();
}

std::unique_ptr<Engine::Graphics::StorageBuffer> Engine::Graphics::OpenGL_Renderer::create_StorageBuffer(std::error_code& ec) const {
	return std::make_unique<gl_SB>();
}

std::unique_ptr<Engine::Graphics::Texture> Engine::Graphics::OpenGL_Renderer::create_Texture(std::error_code& ec) const {
	return std::make_unique<gl_Texture>();
}

std::unique_ptr<Engine::Graphics::TextureArray> Engine::Graphics::OpenGL_Renderer::create_TextureArray(std::error_code& ec) const {
	return std::make_unique<gl_TextureArray>();
}

void Engine::Graphics::OpenGL_Renderer::renderData(
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb
) {
	shader.use_shader();
	va.bind();
	glDrawArrays(GL_TRIANGLES, 0, 0); //not used
}

void Engine::Graphics::OpenGL_Renderer::renderIndexedData(
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t baseIndex
) {
	shader.use_shader();
	va.bind();
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
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t instanceCount
) {
	shader.use_shader();
	va.bind();
	glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
}

void Engine::Graphics::OpenGL_Renderer::renderBasedInstancedData(
	const ShaderProgram& shader,
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