#pragma once
#include <vector>
#include <Rendering/OpenGL_Renderer.h>
#include <Utility/Sprite.h>
#include <Utility/TextBufferBuilder.h>
#include <UI/SDF_Font_Manager.h>
#include <string>

constexpr uint32_t MAX_UI_SPRITES_PER_DRAW = 1000;
constexpr uint32_t MAX_TOOLTIP_TEXT_SIZE = 500;

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
	void update_cursor_item();
	void update_hotbar_active_slot();
	//adds new text to buffer, returns length of added text based on height parameter
	float place_text_to_buffer(std::vector<UI_Vertex2f>& buffer, const char* text, uint32_t text_size, glm::vec2 left_bottom_pos, float height, glm::vec4 color);
	void adjust_tooltip_text_pos(float dX, float dY);
	void update_items();
	void update_craft_slots();
private:
	UI_Renderer() {}
	~UI_Renderer() {}
	SpriteManager* spriteMgr = nullptr;
	Player* main_player_ptr = nullptr;
	SDF_Font_Manager sdf_font_manager;
	TextBufferBuilder text_builder;

	std::unique_ptr<UBO> ui_ubo;
	UI_UBO ubo_data{};
	std::unique_ptr<EBO> ebo;

	std::unique_ptr<ShaderProgram> ui_shader;
	//slots vbo is used for base slots and tooltip
	std::unique_ptr<VAO> slots_vao;
	std::unique_ptr<VBO> slots_vbo;
	std::unique_ptr<VAO> craft_slots_vao;
	std::unique_ptr<VBO> craft_slots_vbo;
	//items vbo is used for items and cursor item
	std::unique_ptr<VAO> items_vao;
	std::unique_ptr<VBO> items_vbo;

	std::unique_ptr<ShaderProgram> sdf_text_shader;
	//text vbo is used for items text and tooltip or cursor item text
	std::unique_ptr<VAO> text_vao;
	std::unique_ptr<VBO> text_vbo;

	std::vector<UI_Vertex2f> basic_slot_buffer;
	std::vector<UI_Vertex2f> active_hotbar_slot_buffer;
	std::vector<UI_Vertex2f> chest_slot_buffer;
	std::vector<UI_Vertex2f> craft_slot_buffer;
	std::vector<UI_Vertex2f> craft_borders_buffer;
	std::vector<UI_Vertex2f> helper_craft_slot_buffer;
	std::vector<UI_Vertex2f> tooltip_buffer;

	std::vector<UI_Vertex2f> basic_items_buffer;
	std::vector<UI_Vertex2f> chest_items_buffer;
	std::vector<UI_Vertex2f> cursor_item_buffer;

	std::vector<UI_Vertex2f> basic_text_buffer;
	std::vector<UI_Vertex2f> chest_text_buffer;
	std::vector<UI_Vertex2f> tooltip_text_buffer;
	std::vector<UI_Vertex2f> cursor_item_text_buffer;

	uint32_t slots_INDEX_SIZE = 0;
	uint32_t base_slots_INDEX_SIZE = 0;
	uint32_t slots_VERTEX_SIZE = 0;
	static uint32_t tooltip_slots_INDEX_SIZE;
	uint32_t craft_slots_INDEX_SIZE = 0;
	uint32_t helper_slots_INDEX_SIZE = 0;

	uint32_t items_INDEX_SIZE = 0;
	uint32_t items_hotbar_INDEX_SIZE = 0;
	uint32_t items_VERTEX_SIZE = 0;
	uint32_t chest_items_INDEX_SIZE = 0;
	uint32_t chest_items_VERTEX_SIZE = 0;
	uint32_t TOTAL_ITEMS_INDEX_SIZE = 0;
	uint32_t TOTAL_ITEMS_VERTEX_SIZE = 0;

	uint32_t text_INDEX_SIZE = 0;
	uint32_t text_hotbar_INDEX_SIZE = 0;
	uint32_t text_VERTEX_SIZE = 0;
	uint32_t chest_text_INDEX_SIZE = 0;
	uint32_t chest_text_VERTEX_SIZE = 0;
	uint32_t TOTAL_TEXT_INDEX_SIZE = 0;
	uint32_t TOTAL_TEXT_VERTEX_SIZE = 0;

	uint32_t tooltip_text_INDEX_SIZE = 0;
	uint32_t cursor_text_INDEX_SIZE = 0;

	float tooltip_frame_size = 0.02f;
	float tooltip_text_height = 0.04f;
};