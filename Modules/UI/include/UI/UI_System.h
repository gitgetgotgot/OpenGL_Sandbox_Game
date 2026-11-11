#pragma once
#include <Rendering/Interfaces/Renderer.h>
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
		void render(std::unique_ptr<Engine::Graphics::Renderer>& renderer);
	private:
		UI_System() = default;
		~UI_System() = default;

		std::unique_ptr<Engine::Graphics::UniformBuffer> ub;
		UI_UBO ubo_data{};
		std::unique_ptr<Engine::Graphics::IndexBuffer> ib;

		std::unique_ptr<Engine::Graphics::ShaderProgram> sprites_shader;
		std::unique_ptr<Engine::Graphics::ShaderProgram> tiled_sprites_shader;
		std::unique_ptr<Engine::Graphics::VertexArray> sprites_va;
		std::unique_ptr<Engine::Graphics::VertexBuffer> sprites_vb;

		std::unique_ptr<Engine::Graphics::ShaderProgram> sdf_text_shader;
		std::unique_ptr<Engine::Graphics::VertexArray> sdf_text_va;
		std::unique_ptr<Engine::Graphics::VertexBuffer> sdf_text_vb;

		UI_RenderContext render_context;
		uint32_t sprites_INDEX_OFFSET = 0;
		uint32_t sdf_text_INDEX_OFFSET = 0;
	};
}