#pragma once
#include <vector>
#include <Rendering/OpenGL_Renderer.h>
#include <Utility/Sprite.h>
#include <Utility/TextBufferBuilder.h>
#include <UI/SDF_Font_Manager.h>
#include <string>

constexpr uint32_t MAX_UI_SPRITES_PER_DRAW = 1000;

struct UI_Vertex2f {
	UI_Vertex2f() {}
	UI_Vertex2f(glm::vec2 pos, glm::vec2 UV) : pos{ pos }, UV{ UV } {}
	UI_Vertex2f(glm::vec2 pos, glm::vec2 UV, glm::vec4 color) : pos{ pos }, UV{ UV }, color{ color } {}
	glm::vec2 pos{ 0.0f };
	glm::vec2 UV{ 0.0f };
	glm::vec4 color{ 1.0f };
};

struct UI_UBO {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};

struct Player;

class UI_Renderer {
public:
	static UI_Renderer* get_instance() {
		static UI_Renderer ui;
		return &ui;
	}
	void init(Player* player);
	void update();
	void render(std::unique_ptr<OpenGL_Renderer>& renderer);

	void init_basic_inventory_slots_data();
	void update_tooltip_data();
	void place_text_to_buffer(std::string text, glm::vec2 left_bottom_pos, float height, glm::vec4 color);
	void update_items();
	void update_items_text();
private:
	UI_Renderer() {}
	~UI_Renderer() {}
	SpriteManager* spriteMgr = nullptr;
	Player* main_player_ptr = nullptr;
	SDF_Font_Manager sdf_font_manager;
	TextBufferBuilder text_builder;

	std::unique_ptr<UBO> ui_ubo;
	UI_UBO ubo_data;
	std::unique_ptr<EBO> ebo;

	std::unique_ptr<ShaderProgram> ui_shader;
	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;

	std::unique_ptr<VAO> items_vao;
	std::unique_ptr<VBO> items_vbo;

	std::unique_ptr<ShaderProgram> sdf_text_shader;
	std::unique_ptr<VAO> text_vao;
	std::unique_ptr<VBO> text_vbo;

	std::vector<UI_Vertex2f> basic_slot_vertices;
	std::vector<UI_Vertex2f> chest_slot_vertices;
	std::vector<UI_Vertex2f> tooltip_vertices;
	std::vector<UI_Vertex2f> slot_items_vertices;
	std::vector<UI_Vertex2f> sdf_text_buffer;

	uint32_t current_slots_buffer_index_size = 0;
	uint32_t current_slots_buffer_size = 0;
	uint32_t current_items_buffer_index_size = 0;
	uint32_t current_items_buffer_size = 0;
	uint32_t current_items_buffer_hotbar_index_size = 0;
	uint32_t current_text_buffer_index_size = 0;
	uint32_t current_text_buffer_size = 0;
	uint32_t current_text_buffer_hotbar_index_size = 0;
	float tooltip_frame_size = 0.02f;
};