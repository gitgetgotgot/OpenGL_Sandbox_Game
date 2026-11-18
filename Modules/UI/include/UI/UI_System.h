#pragma once
#include <Rendering/RHI/Renderer.h>
#include "UI/UI_Data.h"
#include <vector>

namespace Engine::UI {
	class UI_System {
		static constexpr uint16_t MAX_SPRITES_PER_DRAW = 2000;
		static constexpr uint16_t MAX_SDF_TEXT_PER_DRAW = 2000;
	public:
		static UI_System& Instance() {
			static UI_System ui;
			return ui;
		}
		bool init();
		void uninit();
		void update();
		void render(Engine::Graphics::Renderer* renderer);
	private:
		UI_System() = default;
		~UI_System() = default;

		UI_UBO ubo_data{};
		// common uniform buffer
		std::unique_ptr<Engine::Graphics::UniformBuffer> common_ub;
		// common index buffer for triangles
		std::unique_ptr<Engine::Graphics::IndexBuffer> common_ib;

		std::unique_ptr<Engine::Graphics::Pipeline> sprites_pipeline;
		std::unique_ptr<Engine::Graphics::Pipeline> tiled_sprites_pipeline;
		std::unique_ptr<Engine::Graphics::Pipeline> sdf_text_pipeline;

		std::unique_ptr<Engine::Graphics::ShaderHandle> sprites_shader;
		std::unique_ptr<Engine::Graphics::ShaderHandle> tiled_sprites_shader;
		std::unique_ptr<Engine::Graphics::ShaderHandle> sdf_text_shader;

		std::unique_ptr<Engine::Graphics::VertexLayout> sprites_vl;
		std::unique_ptr<Engine::Graphics::VertexLayout> sdf_text_vl;

		std::unique_ptr<Engine::Graphics::VertexBuffer> sprites_vb;
		std::unique_ptr<Engine::Graphics::VertexBuffer> sdf_text_vb;

		UI_RenderContext render_context;
		uint32_t sprites_INDEX_OFFSET = 0;
		uint32_t sdf_text_INDEX_OFFSET = 0;
	};
}