#include <Rendering/OpenGL_Renderer.h>

OpenGL_Renderer::OpenGL_Renderer(GLFWwindow* window) {
	this->window = window;
	//openGL settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGL_Renderer::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL_Renderer::present() {
	glfwSwapBuffers(window);
}

void OpenGL_Renderer::setViewport(float x, float y, float width, float height) {
	glViewport(x, y, width, height);
}

void OpenGL_Renderer::setViewport(glm::vec2 start_pos, glm::vec2 size) {
	glViewport(start_pos.x, start_pos.y, size.x, size.y);
}

void OpenGL_Renderer::useDepthTest(bool use) {
	if (use) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void OpenGL_Renderer::useDepthWrite(bool use) {
	glDepthMask(use);
}

void OpenGL_Renderer::useDepthFunc(GLenum func) {
	glDepthFunc(func);
}

void OpenGL_Renderer::useScissorTest(bool use) {
	if (use) glEnable(GL_SCISSOR_TEST);
	else glDisable(GL_SCISSOR_TEST);
}

void OpenGL_Renderer::setScissorRect(float x, float y, float width, float height) {
	glScissor(x, y, width, height);
}

void OpenGL_Renderer::setScissorRect(glm::vec2 start_pos, glm::vec2 size) {
	glScissor(start_pos.x, start_pos.y, size.x, size.y);
}

void OpenGL_Renderer::renderData(
	std::unique_ptr<ShaderProgram>& shader,
	std::unique_ptr<VAO>& va,
	std::unique_ptr<VBO>& vb
) {
	shader->use_shader();
	va->bind_VAO();
	glDrawArrays(GL_TRIANGLES, 0, 0); //not used
}

void OpenGL_Renderer::renderIndexedData(
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

void OpenGL_Renderer::renderIndexedDataVAOSpecific(
	std::unique_ptr<VAO>& vao,
	uint32_t indexCount
) {
	vao->bind_VAO();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void OpenGL_Renderer::renderInstancedData(
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

void OpenGL_Renderer::renderBasedInstancedData(
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