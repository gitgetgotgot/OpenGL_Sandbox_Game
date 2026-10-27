#include "Core/WorldManager.h"
#include "Core/Menu/SavesPage.h"
#include <Objects/ObjectManager.h>

void WorldManager::create_world(const std::string& name, const World::WorldOptions& options) {
	world_created = false;
	worker_thread.reset();
	worker_thread = std::make_unique<std::jthread>(&WorldManager::_world_generator, this, name, options);
}

void WorldManager::load_world(const std::filesystem::path& save_file_path) {
	world_loaded = false;

	std::string world_name = save_file_path.filename().string();
	worlds[world_name] = std::make_unique<World>();
	worlds[world_name]->init_render_buffers();

	worker_thread.reset();
	worker_thread = std::make_unique<std::jthread>(&WorldManager::_world_loader, this, save_file_path);
}

void WorldManager::save_world(const std::filesystem::path& save_file_path) {

}

void WorldManager::switch_active_world(const std::string& name) {
	active_world = worlds[name].get();
}

void WorldManager::update_active_world_buffers() {
	if (active_world) active_world->init_chunk_buffers();
}

void WorldManager::update() {
	if (active_world) active_world->update();
}

void WorldManager::render(std::unique_ptr<OpenGL_Renderer>& renderer) {
	if (active_world) active_world->render(renderer);
}

void WorldManager::clear() {
	for (auto& [name, world] : worlds) {
		world.reset();
	}
}


bool WorldManager::world_is_created() {
	return world_created;
}

bool WorldManager::world_is_saved() {
	return world_saved;
}

bool WorldManager::world_is_loaded() {
	return world_loaded;
}


void WorldManager::_world_generator(const std::string& name, const World::WorldOptions& options) {
	// set world options
	// world has 64x64 chunks and its size can be divided by 64 without leftovers
	uint16_t width = 4224, height = 1216;
	if (options.size == World::WorldOptions::WorldSize::SMALL) {
		width = 4224; height = 1216;
	}
	else if (options.size == World::WorldOptions::WorldSize::MEDIUM) {
		width = 6336; height = 1856;
	}
	else if (options.size == World::WorldOptions::WorldSize::LARGE) {
		width = 8448; height = 2432;
	}
	else if(options.size == World::WorldOptions::WorldSize::LARGE_PLUS_PLUS) {
		width = 12672; height = 3648;
	}

	WorldSlot* world_slots = new WorldSlot[width * height];
	std::unordered_map<uint32_t, std::unique_ptr<CoreObject::ObjectComponent>> components;

	_build_world(width, height, world_slots, components);

	_save_world_data(name, width, height, world_slots, components);

	delete[] world_slots;
	//std::this_thread::sleep_for(std::chrono::seconds(2));
	world_created = true;
}

void WorldManager::_world_loader(const std::filesystem::path& save_file_path) {
	std::string world_name = save_file_path.filename().string();
	World* world = worlds[world_name].get();

	std::filesystem::path file_path;
	// meta file
	{
		file_path = save_file_path / "meta.save";
		std::ifstream in(file_path, std::ios::binary);
		//write world size
		in.read(reinterpret_cast<char*>(&world->width), sizeof(uint16_t));
		in.read(reinterpret_cast<char*>(&world->height), sizeof(uint16_t));
		world->init_tiles_buffer();
	}
	// tiles file
	{
		file_path = save_file_path / "tiles.save";
		std::ifstream in(file_path, std::ios::binary);

		// 1. make up-to-date ID table
		std::vector<uint16_t> table;

		uint32_t table_size;
		in.read(reinterpret_cast<char*>(&table_size), sizeof(uint32_t));
		table.resize(table_size);

		for (uint32_t i = 0; i < table_size; ++i) {
			// UID will never be more than 256 characters
			uint8_t UID_size;
			in.read(reinterpret_cast<char*>(&UID_size), sizeof(uint8_t));
			std::string UID;
			UID.resize(UID_size);
			in.read(UID.data(), UID_size);
			auto correct_ID = CoreObject::ObjectManager::get_instance().get_object_id(UID);
			if (correct_ID.has_value()) {
				table[i] = correct_ID.value();
			}
			else {
				table[i] = 0; // just place "Core:Air" for now
			}
		}

		// 2. read tiles
		for (uint16_t x = 0; x < world->width; ++x) {
			for (uint16_t y = 0; y < world->height; ++y) {
				WorldSlot slot;
				in.read(reinterpret_cast<char*>(&slot), sizeof(WorldSlot));
				slot.tile_id = table[slot.tile_id];
				slot.wall_id = table[slot.wall_id];
				world->world_slots[y * world->width + x] = slot;
			}
		}
	}
	switch_active_world(world_name);
	//std::this_thread::sleep_for(std::chrono::seconds(2));
	world_loaded = true;
}

void WorldManager::_world_saver(const std::filesystem::path& save_file_path) {

	std::this_thread::sleep_for(std::chrono::seconds(2));
	world_saved = true;
}

void WorldManager::_build_world(
	uint16_t world_width, uint16_t world_height,
	WorldSlot* world_slots,
	std::unordered_map<uint32_t, std::unique_ptr<CoreObject::ObjectComponent>>& components
) {
	// build simple flat world for now
	/*
	int biome_width = width / 10;
	//generate flat surface with tiny waves
	for (int i = 0; i < width; i++) {
		int j = 0;
		//int point = 891 + sin(0.1 * i) * 20;
		int point = 899 + sin(i * 0.05) * 1;
		for (; j < point; j++) {  //stone layer
			world_slots[j * width + i].tile_id = 2;
		}
		while (j < point + 8) { //dirt layer
			world_slots[j * width + i].tile_id = 3;
			j++;
		}
		world_slots[j * width + i].tile_id = 4; //dirt with grass
	}
	//generate some kind of desert on either left or right side of the map
	bool desertIsOnLeft = rand() % 2;
	int left_biome_border, right_biome_border;
	if (desertIsOnLeft) {
		left_biome_border = width * 0.3 - 1;
		right_biome_border = left_biome_border + biome_width;
	}
	else {
		left_biome_border = width * 0.6 - 1;
		right_biome_border = left_biome_border + biome_width;
	}
	float value = 0.f;
	for (int i = left_biome_border; i < right_biome_border; i++) {
		int point = 904 + sin(3.14159 * value / 180.) * 15;
		for (int j = 600; j < point; j++) {
			world_slots[j * width + i].tile_id = 5;
		}
		value += 180 / 420.f;
	}
	//generate oceans on both sides
	left_biome_border = 0;
	right_biome_border = left_biome_border + width * 0.05;
	value = 180.f;
	for (int i = left_biome_border; i < right_biome_border - 20; i++) { //place sand in a curve line and fill with water blocks on left side
		int point = 829 + (cos(3.14159 * value / 180.) + 1) * 80;
		int j = point - 40;
		for (; j < point; j++) {  //place sand
			world_slots[j * width + i].tile_id = 5;
		}
		while (j <= 905) {  //place water
			world_slots[j * width + i].tile_id = 13;
			j++;
		}
		while (j <= 907) {  //remove redundant blocks
			if (world_slots[j * width + i].tile_id > 0) {
				if (world_slots[j * width + i].tile_id != 5) {
					world_slots[j * width + i].tile_id = 0;
				}
			}
			j++;
		}
		value += 90 / 190.f;
	}
	for (int i = right_biome_border - 20; i < right_biome_border; i++) { //make straigh place for 20 blocks after the ocean
		int point = 908;
		for (int j = point - 40; j < point; j++) {
			world_slots[j * width + i].tile_id = 5;
		}
	}
	right_biome_border = width - 1;
	left_biome_border = right_biome_border - width * 0.05;
	value = 90.f;
	for (int i = left_biome_border; i <= right_biome_border; i++) { //place sand in a curve line and fill with water blocks on right side
		int point = 829 + (cos(3.14159 * value / 180.) + 1) * 80;
		int j = point - 40;
		for (; j < point; j++) {  //place sand
			world_slots[j * width + i].tile_id = 5;
		}
		while (j <= 905) {  //place water
			world_slots[j * width + i].tile_id = 13;
			j++;
		}
		while (j <= 907) {  //remove redundant blocks
			if (world_slots[j * width + i].tile_id > 0) {
				if (world_slots[j * width + i].tile_id != 5) {
					world_slots[j * width + i].tile_id = 0;
				}
			}
			j++;
		}
		value += 90 / 190.f;
	}
	for (int i = left_biome_border - 20; i < left_biome_border; i++) { //make straigh place for 20 blocks after the ocean
		int point = 908;
		for (int j = point - 40; j < point; j++) {
			world_slots[j * width + i].tile_id = 5;
		}
	}
	*/

	uint16_t stone_tile_id = CoreObject::ObjectManager::get_instance().get_object_id("Item:Core:Stone").value();
	uint16_t dirt_tile_id = CoreObject::ObjectManager::get_instance().get_object_id("Item:Core:Dirt").value();
	uint16_t grass_dirt_tile_id = CoreObject::ObjectManager::get_instance().get_object_id("Item:Core:Grass Dirt").value();

	uint16_t ground_height = uint16_t(float(world_height) * 0.75f); //world ground is 75% of world height
	uint16_t dirt_layer_height = 5;

	//fill stone layer
	uint16_t stone_layer_endpoint = ground_height - 1 - dirt_layer_height;
	for (uint16_t x = 0; x < world_width; ++x) {
		for (uint16_t y = 0; y < stone_layer_endpoint; ++y) {
			world_slots[y * world_width + x].tile_id = stone_tile_id;
		}
	}
	//fill dirt layer
	uint16_t dirt_layer_endpoint = ground_height - 1;
	for (uint16_t x = 0; x < world_width; ++x) {
		for (uint16_t y = ground_height - 1 - dirt_layer_height; y < dirt_layer_endpoint; ++y) {
			world_slots[y * world_width + x].tile_id = dirt_tile_id;
		}
	}
	//fill grass dirt layer
	uint16_t grass_dirt_layer = ground_height - 1;
	for (uint16_t x = 0; x < world_width; ++x) {
		for (uint16_t y = ground_height - 1; y < ground_height; ++y) {
			world_slots[grass_dirt_layer * world_width + x].tile_id = grass_dirt_tile_id;
		}
	}
}

void WorldManager::_save_world_data(
	const std::string& name,
	const uint16_t world_width, const uint16_t world_height,
	const WorldSlot* world_slots,
	const std::unordered_map<uint32_t, std::unique_ptr<CoreObject::ObjectComponent>>& components
) {
	std::filesystem::path root = RESOURCE_ROOT;
	std::filesystem::path saves_path = root / "Saves";
	if (!std::filesystem::exists(saves_path)) {
		std::filesystem::create_directory(saves_path);
	}
	std::filesystem::path save_path = saves_path / name;
	std::filesystem::create_directory(save_path);

	std::filesystem::path file_path;
	// meta file
	{
		file_path = save_path / "meta.save";
		std::ofstream out(file_path, std::ios::binary);
		//write world size
		out.write(reinterpret_cast<const char*>(&world_width), sizeof(uint16_t));
		out.write(reinterpret_cast<const char*>(&world_height), sizeof(uint16_t));
	}
	// tiles file
	{
		file_path = save_path / "tiles.save";
		std::ofstream out(file_path, std::ios::binary);

		// 1. write ID_to_UID table
		std::vector<std::string_view> ID_to_UID_table;
		CoreObject::ObjectManager::get_instance().build_ID_to_UID_table(ID_to_UID_table);

		uint32_t table_size = ID_to_UID_table.size();
		out.write(reinterpret_cast<const char*>(&table_size), sizeof(uint32_t));

		for (uint32_t i = 0; i < table_size; ++i) {
			const std::string_view& UID = ID_to_UID_table[i];
			// UID will never be more than 256 characters
			const uint8_t UID_size = static_cast<uint8_t>(UID.size());
			out.write(reinterpret_cast<const char*>(&UID_size), sizeof(uint8_t));
			out.write(UID.data(), UID_size);
		}

		// 2. write tiles
		for (uint16_t x = 0; x < world_width; ++x) {
			for (uint16_t y = 0; y < world_height; ++y) {
				out.write(reinterpret_cast<const char*>(&world_slots[y * world_width + x]), sizeof(WorldSlot));
			}
		}
	}
	// components file
	{
		file_path = save_path / "components.save";
		std::ofstream out(file_path, std::ios::binary);
		
	}
	// entities file
	{
		file_path = save_path / "entities.save";
		std::ofstream out(file_path, std::ios::binary);
		// write entities data
	}

	SavesPage::Instance().load_save(name);
}