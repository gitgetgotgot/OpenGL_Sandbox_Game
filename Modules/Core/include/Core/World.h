#pragma once
#include <Entities/EntitySystem.h>
#include <Utility/SparseSet.h>
#include <IOSystem/SystemContext.h>
#include <unordered_map>

struct Vertex2f {
	glm::vec2 pos;
	glm::vec2 UV;
};

struct ChunkRenderBuffer {
	uint32_t x = 0, y = 0;
	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
	sparse_set<Vertex2f> vertices;
	uint32_t index_count = 0;
};

//renderable chunks are used in this order:
//		3	7	11	15
//		2	6	10	14
//		1	5	9	13
//		0	4	8	12

class World {
public:
	void init(Player* player);
	void update();
	void render(std::unique_ptr<OpenGL_Renderer>& renderer);

	void update_chunks(uint32_t& camera_chunk_x, uint32_t& camera_chunk_y);
	void load_chunk_buffer(uint32_t chunk_world_x, uint32_t chunk_world_y, uint32_t chunk_buffer_index);
	void load_chunk_buffers(int x_move, int y_move);

	void process_LB_click();
	void process_RB_click();
	void process_scrollwheel();

	void place_tile(uint32_t world_x, uint32_t world_y, uint16_t tile_id);
	void destroy_tile(uint32_t world_x, uint32_t world_y);

	void generate_world_data();
	void save_world_data_in_file(const char* filePath);
	void load_world_data_from_file(const char* filePath);
	~World();
private:
	uint16_t width = 4224;
	uint16_t height = 1216;

	SpriteManager* spriteMgr = nullptr;
	WorldSlot* world_slots = nullptr;
	std::unordered_map<uint32_t, std::unique_ptr<ObjectComponent>> object_components;

	static const uint32_t RENDERABLE_CHUNKS_SIZE = 16;
	ChunkRenderBuffer* render_buffers[RENDERABLE_CHUNKS_SIZE];

	uint32_t last_chunk_x = 0;
	uint32_t last_chunk_y = 0;
	int
		current_buffer_x_min = 0,
		current_buffer_x_max = 3,
		current_buffer_y_min = 0,
		current_buffer_y_max = 3;
	int MAX_CHUNK_X, MAX_CHUNK_Y;

	std::unique_ptr<ShaderProgram> shader;
	std::unique_ptr<UBO> sprites_ubo;
	Universal_UBO_Data sprites_ubo_data;
	std::unique_ptr<EBO> ebo;
	std::unique_ptr<SSBO> spriteLightMapSSBO;
	ShaderLightingInfo* lightMap_data = nullptr;

	Camera main_camera;
	float MAX_ZOOM = 33.8f, MIN_ZOOM = 6.8f;
	float speed = 5.f;
	Player* main_player_ptr = nullptr;
};