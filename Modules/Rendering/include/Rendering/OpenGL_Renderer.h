#pragma once
#include "Rendering/ShaderClass.h"
#include "Rendering/VAO.h"
#include "Rendering/VBO.h"
#include "Rendering/EBO.h"
#include "Rendering/UBO.h"
#include "Rendering/SSBO.h"
#include "Rendering/Texture.h"
#include "Rendering/TextureArray.h"
#include <memory>

namespace Engine::Graphics {
	class OpenGL_Renderer {
	public:
		inline static OpenGL_Renderer* renderer_ptr = nullptr;
		enum class DepthFunc : uint8_t {
			qwerty
		};
		bool init();
		void clear(float r, float g, float b);
		void end_frame();
		void setViewport(float x, float y, float width, float height);
		void useDepthTest(bool use);
		void useDepthWrite(bool use);
		void useDepthFunc(DepthFunc func);
		void useScissorTest(bool use);
		void setScissorRect(float x, float y, float width, float height);
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
	};
}