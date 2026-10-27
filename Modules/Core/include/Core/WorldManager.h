#pragma once
#include "Core/World.h"
#include <filesystem>
#include <thread>

class WorldManager {
public:
    static WorldManager& Instance() {
        static WorldManager mgr;
        return mgr;
    }
    void create_world(const std::string& name, const World::WorldOptions& options);
    void load_world(const std::filesystem::path& save_file_path);
    void save_world(const std::filesystem::path& save_file_path);
    void switch_active_world(const std::string& name);
    void update_active_world_buffers();
    void update();
    void render(std::unique_ptr<Engine::Graphics::OpenGL_Renderer>& renderer);
    void clear();

    bool world_is_created();
    bool world_is_saved();
    bool world_is_loaded();
private:
    void _world_generator(const std::string& name, const World::WorldOptions& options);
    void _world_loader(const std::filesystem::path& save_file_path);
    void _world_saver(const std::filesystem::path& save_file_path);

    void _build_world(
        uint16_t world_width, uint16_t world_height,
        WorldSlot* world_slots,
        std::unordered_map<uint32_t, std::unique_ptr<Engine::Object::ObjectComponent>>& components
    );
    void _save_world_data(
        const std::string& name,
        const uint16_t world_width, const uint16_t world_height,
        const WorldSlot* world_slots,
        const std::unordered_map<uint32_t, std::unique_ptr<Engine::Object::ObjectComponent>>& components
    );

    WorldManager() = default;
    ~WorldManager() = default;
    std::unordered_map<std::string, std::unique_ptr<World>> worlds;
    World* active_world = nullptr;

    std::unique_ptr<std::jthread> worker_thread;
    std::atomic_bool world_created = false;
    std::atomic_bool world_loaded = false;
    std::atomic_bool world_saved = false;
};