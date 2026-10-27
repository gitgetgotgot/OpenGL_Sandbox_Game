#pragma once
#include <Rendering/OpenGL_Renderer.h>
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
		void init();
		void uninit();
		void update();
		void render(std::unique_ptr<Engine::Graphics::OpenGL_Renderer>& renderer);
	private:
		UI_System() = default;
		~UI_System() = default;

		std::unique_ptr<Engine::Graphics::UBO> ubo;
		UI_UBO ubo_data{};
		std::unique_ptr<Engine::Graphics::EBO> ebo;

		std::unique_ptr<Engine::Graphics::ShaderProgram> sprites_shader;
		std::unique_ptr<Engine::Graphics::ShaderProgram> tiled_sprites_shader;
		std::unique_ptr<Engine::Graphics::VAO> sprites_vao;
		std::unique_ptr<Engine::Graphics::VBO> sprites_vbo;

		std::unique_ptr<Engine::Graphics::ShaderProgram> sdf_text_shader;
		std::unique_ptr<Engine::Graphics::VAO> sdf_text_vao;
		std::unique_ptr<Engine::Graphics::VBO> sdf_text_vbo;

		UI_RenderContext render_context;
		uint32_t sprites_INDEX_OFFSET = 0;
		uint32_t sdf_text_INDEX_OFFSET = 0;
	};
}