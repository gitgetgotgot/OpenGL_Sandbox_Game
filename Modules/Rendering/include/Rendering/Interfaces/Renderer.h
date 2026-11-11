#pragma once
#include <Rendering/Interfaces/ShaderProgram.h>
#include <Rendering/Interfaces/VertexLayout.h>
#include <Rendering/Interfaces/VertexBuffer.h>
#include <Rendering/Interfaces/IndexBuffer.h>
#include <Rendering/Interfaces/UniformBuffer.h>
#include <Rendering/Interfaces/StorageBuffer.h>
#include <Rendering/Interfaces/Texture.h>
#include <Rendering/Interfaces/TextureArray.h>
#include <system_error>
#include <memory>

namespace Engine::Graphics {
	class Renderer {
	public:
		// instance, used in DeviceContext to update viewport
		inline static Renderer* instance = nullptr;

		enum class DepthFunc : uint8_t { Less, LessOrEqual, Greater, GreaterOrEqual };

		Renderer() { instance = this; }
		virtual ~Renderer() { instance = nullptr; }

		virtual void clear(float r, float g, float b) = 0;
		virtual void end_frame() = 0;

		virtual void setViewport(float x, float y, float width, float height) = 0;
		virtual void useDepthTest(bool use) = 0;
		virtual void useDepthWrite(bool use) = 0;
		virtual void useDepthFunc(DepthFunc func) = 0;
		virtual void useScissorTest(bool use) = 0;
		virtual void setScissorRect(float x, float y, float width, float height) = 0;

		virtual std::unique_ptr<ShaderProgram> create_ShaderPr(
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path,
			std::error_code& ec
		) const = 0;
		virtual std::unique_ptr<VertexLayout> create_VertexLayout(std::error_code& ec) const = 0;
		virtual std::unique_ptr<VertexBuffer> create_VertexBuffer(std::error_code& ec) const = 0;
		virtual std::unique_ptr<IndexBuffer> create_IndexBuffer(std::error_code& ec) const = 0;
		virtual std::unique_ptr<UniformBuffer> create_UniformBuffer(std::error_code& ec) const = 0;
		virtual std::unique_ptr<StorageBuffer> create_StorageBuffer(std::error_code& ec) const = 0;
		virtual std::unique_ptr<Texture> create_Texture(std::error_code& ec) const = 0;
		virtual std::unique_ptr<TextureArray> create_TextureArray(std::error_code& ec) const = 0;

		virtual void renderData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb
		) = 0;
		virtual void renderIndexedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t baseIndex = 0
		) = 0;
		virtual void renderIndexedDataVAOSpecific(
			const VertexLayout& va,
			uint32_t indexCount
		) = 0;
		virtual void renderInstancedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount
		) = 0;
		virtual void renderBasedInstancedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t baseInstance
		) = 0;
	};
}