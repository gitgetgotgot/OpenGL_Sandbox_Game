#include "Rendering/OpenGL_Renderer.h"
#include <glad/gl.h>
#include <glm/ext/vector_float2.hpp>
#include <Utility/Logger.h>

bool Engine::Graphics::OpenGL_Renderer::init() {
	//openGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// read texture bytes without alignment or padding (correctly load textures of different formats and sizes)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	return true;
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

void Engine::Graphics::OpenGL_Renderer::renderData(
	std::unique_ptr<ShaderProgram>& shader,
	std::unique_ptr<VAO>& va,
	std::unique_ptr<VBO>& vb
) {
	shader->use_shader();
	va->bind_VAO();
	glDrawArrays(GL_TRIANGLES, 0, 0); //not used
}

void Engine::Graphics::OpenGL_Renderer::renderIndexedData(
	std::unique_ptr<ShaderProgram>& shader,
	std::unique_ptr<VAO>& va,
	std::unique_ptr<VBO>& vb,
	std::unique_ptr<EBO>& ib,
	uint32_t indexCount,
	uint32_t baseIndex
) {
	shader->use_shader();
	va->bind_VAO();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(baseIndex * sizeof(uint32_t)));
}

void Engine::Graphics::OpenGL_Renderer::renderIndexedDataVAOSpecific(
	std::unique_ptr<VAO>& vao,
	uint32_t indexCount
) {
	vao->bind_VAO();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void Engine::Graphics::OpenGL_Renderer::renderInstancedData(
	std::unique_ptr<ShaderProgram>& shader,
	std::unique_ptr<VAO>& va,
	std::unique_ptr<VBO>& vb,
	std::unique_ptr<EBO>& ib,
	uint32_t indexCount,
	uint32_t instanceCount
) {
	shader->use_shader();
	va->bind_VAO();
	glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
}

void Engine::Graphics::OpenGL_Renderer::renderBasedInstancedData(
	std::unique_ptr<ShaderProgram>& shader,
	std::unique_ptr<VAO>& va,
	std::unique_ptr<VBO>& vb,
	std::unique_ptr<EBO>& ib,
	uint32_t indexCount,
	uint32_t instanceCount,
	uint32_t baseInstance
) {
	shader->use_shader();
	va->bind_VAO();
	glDrawArraysInstancedBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, instanceCount, baseInstance);
}