#pragma once

#include <memory>
#include <Rendering/ShaderClass.h>
#include <Rendering/VAO.h>
#include <Rendering/VBO.h>
#include <Rendering/EBO.h>
#include <Rendering/Texture.h>
#include <GLFW/glfw3.h>

class OpenGL_Renderer {
public:
	OpenGL_Renderer(GLFWwindow* window);
	void clear(float r, float g, float b);
	void present();
	void setViewport(float x, float y, float width, float height);
	void setViewport(glm::vec2 start_pos, glm::vec2 size);
	void useDepthTest(bool use);
	void useDepthWrite(bool use);
	void useDepthFunc(GLenum func);
	void useScissorTest(bool use);
	void setScissorRect(float x, float y, float width, float height);
	void setScissorRect(glm::vec2 start_pos, glm::vec2 size);
	void renderData(
		std::unique_ptr<ShaderProgram>& shader,
		std::unique_ptr<VAO>& va,
		std::unique_ptr<VBO>& vb
	);
	void renderIndexedData(
		std::unique_ptr<ShaderProgram>& shader,
		std::unique_ptr<VAO>& va,
		std::unique_ptr<VBO>& vb,
		std::unique_ptr<EBO>& ib,
		uint32_t indexCount,
		uint32_t baseIndex = 0
	);
	void renderIndexedDataVAOSpecific(
		std::unique_ptr<VAO>& vao,
		uint32_t indexCount
	);
	void renderInstancedData(
		std::unique_ptr<ShaderProgram>& shader,
		std::unique_ptr<VAO>& va,
		std::unique_ptr<VBO>& vb,
		std::unique_ptr<EBO>& ib,
		uint32_t indexCount,
		uint32_t instanceCount
	);
	void renderBasedInstancedData(
		std::unique_ptr<ShaderProgram>& shader,
		std::unique_ptr<VAO>& va,
		std::unique_ptr<VBO>& vb,
		std::unique_ptr<EBO>& ib,
		uint32_t indexCount,
		uint32_t instanceCount,
		uint32_t baseInstance
	);
private:
	GLFWwindow* window;
};