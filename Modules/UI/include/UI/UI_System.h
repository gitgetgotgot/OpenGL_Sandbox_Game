#pragma once
#include <Rendering/OpenGL_Renderer.h>
#include <Utility/UI_Data.h>
#include <vector>

namespace CoreUI {
	constexpr uint16_t MAX_SPRITES_PER_DRAW = 2000;
	constexpr uint16_t MAX_SDF_TEXT_PER_DRAW = 2000;

	class UI_System {
	public:
		static UI_System& get_instance() {
			static UI_System ui;
			return ui;
		}
		void init();
		void update();
		void render(std::unique_ptr<OpenGL_Renderer>& renderer);
		uint16_t add_clip_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
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

		std::vector<UI_Vertex2f> sprites_buffer;
		std::vector<UI_Text_Vertex2f> sdf_text_buffer;
		uint32_t sprites_INDEX_OFFSET = 0;
		uint32_t sdf_text_INDEX_OFFSET = 0;

		std::vector<UI_RenderEntry> render_queue;
		std::vector<ClipRectangle> clip_rects;
	};
}