#pragma once
#include <Rendering/RHI/Renderer.h>
#include <d3d12.h>

namespace Engine::Graphics {
	class D3D12_Renderer : public Renderer {
	public:
		D3D12_Renderer(HWND hwnd, std::error_code& ec);
		~D3D12_Renderer();
	public:
		void start_frame(std::error_code& ec) override;
		void clear(float r, float g, float b) override;
		void present_frame(std::error_code& ec) override;
		void on_window_resize(uint32_t window_width, uint32_t window_height, uint8_t mode_code) override;
		void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void use_depth_test(bool use) override;
		void use_depth_write(bool use) override;
		void use_depth_func(DepthFunc func) override;
		void use_scissor_test(bool use) override;
		void set_scissor_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void bind_pipeline(const Pipeline& p) override;

		std::unique_ptr<ShaderHandle> create_ShaderHandle(
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
		std::unique_ptr<Pipeline> create_Pipeline(std::error_code& ec) const override;

		void renderIndexedData(
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t baseIndex = 0
		) override;

		void renderInstancedData(
			const VertexBuffer& vb,
			const IndexBuffer& ib,
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t baseInstance = 0
		) override;
	private:
		HWND hwnd;
	};
}