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
		// global single instance ptr to Renderer interface
		inline static Renderer* instance = nullptr;

		enum class DepthFunc : uint8_t { Less, LessOrEqual, Greater, GreaterOrEqual };
		enum class PresentMode : uint8_t {
			Immediate,	// no Vsync, no limits
			Double,		// classic Vsync, double buffer
			Triple		// triple buffer, low latency
		};

		Renderer() { instance = this; }
		virtual ~Renderer() { instance = nullptr; }

		virtual void start_frame(std::error_code& ec) = 0;
		virtual void clear(float r, float g, float b) = 0;
		virtual void present_frame(std::error_code& ec) = 0;
		virtual void on_window_resize(uint32_t window_width, uint32_t window_height, PresentMode mode) = 0;
		virtual void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void use_depth_test(bool use) = 0;
		virtual void use_depth_write(bool use) = 0;
		virtual void use_depth_func(DepthFunc func) = 0;
		virtual void use_scissor_test(bool use) = 0;
		virtual void set_scissor_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

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