#pragma once
#include <Rendering/Interfaces/Renderer.h>
#include <d3d12.h>

namespace Engine::Graphics {
	class DirectX12_Renderer : public Renderer {
	public:
		DirectX12_Renderer(HWND window_handle, std::error_code& ec);
		~DirectX12_Renderer();
		void clear(float r, float g, float b) override;
		void end_frame() override;
	private:
		void create_instance(std::error_code& ec);
		void create_sufrace(std::error_code& ec);
		void pick_physical_device(std::error_code& ec);
		void create_logical_device(std::error_code& ec);
		void init_VMA(std::error_code& ec);
	public:
		void setViewport(float x, float y, float width, float height) override;
		void useDepthTest(bool use) override;
		void useDepthWrite(bool use) override;
		void useDepthFunc(DepthFunc func) override;
		void useScissorTest(bool use) override;
		void setScissorRect(float x, float y, float width, float height) override;

		std::unique_ptr<ShaderProgram> create_ShaderPr(
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path,
			std::error_code& ec
		) const override;
		std::unique_ptr<VertexLayout> create_VertexLayout(std::error_code& ec) const override;
		std::unique_ptr<VertexBuffer> create_VertexBuffer(std::error_code& ec) const override;
		std::unique_ptr<IndexBuffer> create_IndexBuffer(std::error_code& ec) const override;
		std::unique_ptr<UniformBuffer> create_UniformBuffer(std::error_code& ec) const override;
		std::unique_ptr<StorageBuffer> create_StorageBuffer(std::error_code& ec) const override;
		std::unique_ptr<Texture> create_Texture(std::error_code& ec) const override;
		std::unique_ptr<TextureArray> create_TextureArray(std::error_code& ec) const override;

		void renderData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb
		) override;
		void renderIndexedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t baseIndex = 0
		) override;
		void renderIndexedDataVAOSpecific(
			const VertexLayout& va,
			uint32_t indexCount
		) override;
		void renderInstancedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount
		) override;
		void renderBasedInstancedData(
			const ShaderProgram& shader,
			const VertexLayout& va,
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t baseInstance
		) override;
	private:
		HWND hwnd;
	};
}