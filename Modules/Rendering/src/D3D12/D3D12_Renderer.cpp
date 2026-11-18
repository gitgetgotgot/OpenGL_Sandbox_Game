#include "Rendering/D3D12/D3D12_Renderer.h"
#include <Utility/Logger.h>

Engine::Graphics::D3D12_Renderer::D3D12_Renderer(HWND hwnd, std::error_code& ec) {
	
}

Engine::Graphics::D3D12_Renderer::~D3D12_Renderer() {

}

void Engine::Graphics::D3D12_Renderer::start_frame(std::error_code& ec) {
	
}

void Engine::Graphics::D3D12_Renderer::clear(float r, float g, float b) {
	
}

void Engine::Graphics::D3D12_Renderer::present_frame(std::error_code& ec) {
	
}

void Engine::Graphics::D3D12_Renderer::on_window_resize(uint32_t window_width, uint32_t window_height, uint8_t mode_code) {
	
}

void Engine::Graphics::D3D12_Renderer::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	
}

void Engine::Graphics::D3D12_Renderer::use_depth_test(bool use) {
	
}

void Engine::Graphics::D3D12_Renderer::use_depth_write(bool use) {
	
}

void Engine::Graphics::D3D12_Renderer::use_depth_func(DepthFunc func) {
	
}

void Engine::Graphics::D3D12_Renderer::use_scissor_test(bool use) {
	
}

void Engine::Graphics::D3D12_Renderer::set_scissor_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	
}

void Engine::Graphics::D3D12_Renderer::bind_pipeline(const Pipeline& p) {
	
}


std::unique_ptr<Engine::Graphics::ShaderHandle> Engine::Graphics::D3D12_Renderer::create_ShaderHandle(
	const std::filesystem::path& vertex_shader_path,
	const std::filesystem::path& fragment_shader_path,
	std::error_code& ec
) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::VertexLayout> Engine::Graphics::D3D12_Renderer::create_VertexLayout(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::VertexBuffer> Engine::Graphics::D3D12_Renderer::create_VertexBuffer(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::IndexBuffer> Engine::Graphics::D3D12_Renderer::create_IndexBuffer(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::UniformBuffer> Engine::Graphics::D3D12_Renderer::create_UniformBuffer(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::StorageBuffer> Engine::Graphics::D3D12_Renderer::create_StorageBuffer(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::Texture> Engine::Graphics::D3D12_Renderer::create_Texture(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::TextureArray> Engine::Graphics::D3D12_Renderer::create_TextureArray(std::error_code& ec) const {
	return nullptr;
}

std::unique_ptr<Engine::Graphics::Pipeline> Engine::Graphics::D3D12_Renderer::create_Pipeline(std::error_code& ec) const {
	return nullptr;
}


void Engine::Graphics::D3D12_Renderer::renderIndexedData(
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t baseIndex
) {
	
}

void Engine::Graphics::D3D12_Renderer::renderInstancedData(
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t instanceCount,
	uint32_t baseInstance
) {
	
}