#pragma once
#include <Rendering/OpenGL_Renderer.h>
#include <Utility/UI_Data.h>
#include <vector>

namespace CoreUI {
	constexpr uint16_t MAX_SPRITES_PER_DRAW = 2000;
	constexpr uint16_t MAX_SDF_TEXT_PER_DRAW = 2000;

	class UI_System {
	public:
		static UI_System& Instance() {
			static UI_System ui;
			return ui;
		}
		void init();
		void update();
		void render(std::unique_ptr<OpenGL_Renderer>& renderer);
	private:
		UI_System() {}
		~UI_System() {}

		std::unique_ptr<UBO> ubo;
		UI_UBO ubo_data{};
		std::unique_ptr<EBO> ebo;

		std::unique_ptr<ShaderProgram> sprites_shader;
		std::unique_ptr<VAO> sprites_vao;
		std::unique_ptr<VBO> sprites_vbo;

		std::unique_ptr<ShaderProgram> sdf_text_shader;
		std::unique_ptr<VAO> sdf_text_vao;
		std::unique_ptr<VBO> sdf_text_vbo;

		UI_RenderContext render_context;
		uint32_t sprites_INDEX_OFFSET = 0;
		uint32_t sdf_text_INDEX_OFFSET = 0;
	};
}