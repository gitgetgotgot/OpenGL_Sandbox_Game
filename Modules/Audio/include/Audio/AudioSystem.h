#pragma once
#include <vector>
#include <deque>
#include <optional>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include <miniaudio.h>

constexpr uint32_t AUDIO_MAX_SOUNDS = 64;

struct AudioResource {
    std::vector<float> pcm_data;
    ma_uint64 frames_read = 1;
    ma_uint32 sample_rate = 48000;
    ma_uint16 channels = 2;
    uint8_t current_instance_count = 0;
    uint8_t MAX_INSTANCE_COUNT = 8;
};

struct PoolSound {
    ma_sound sound;
    ma_audio_buffer audio_buffer;
    uint32_t sound_id = 0;
    uint8_t generation = 1;
    bool is_used = false;
};

struct Sound_Instance_ID {
    uint8_t instance_id = 0;
    uint8_t generation = 1;
};

class AudioSystem {
public:
    static AudioSystem& Instance() {
        static AudioSystem sys;
        return sys;
    }
    void init();
    void uninit();
    void update();

    void play_audio_directly(const std::filesystem::path& path);
    void play_audio_stream(const std::filesystem::path& path);
    void play_audio_stream(uint32_t audio_ID);
    Sound_Instance_ID play_audio_pcm(uint32_t audio_ID, bool loop);

    bool sound_is_playing(uint8_t instance, uint8_t gen);
    void stop_sound(uint8_t instance, uint8_t gen);
    void free_sound(uint8_t instance);

    void load_audio_as_pcm(const std::filesystem::path& filePath, std::string UID);
    void load_audio_as_stream(const std::filesystem::path& filePath);

    void set_global_volume(float volume);
    std::optional<uint32_t> get_sound_id(std::string_view UID) const;
private:
    AudioSystem() = default;
    ~AudioSystem() = default;
    ma_engine audio_engine;

    std::vector<PoolSound> sounds_pool;
    std::vector<uint32_t> free_sounds;

    std::deque<std::string> audio_UID;
    std::unordered_map<std::string_view, uint32_t> UID_to_ID;
    std::vector<std::unique_ptr<AudioResource>> audio_pcm_database;
};