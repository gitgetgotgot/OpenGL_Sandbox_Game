#pragma once
#include <unordered_map>
#include <deque>
#include <optional>
#include <memory>
#include <mutex>
#include "Audio/AudioData.h"

namespace Engine::Util {
    class DataParser;
}

namespace Engine::Audio {
    class AudioSystem {
        static constexpr uint32_t MAX_SOUNDS_POOL_SIZE = 64;
        static constexpr uint32_t MAX_STREAM_SOUNDS_POOL_SIZE = 8;
    public:
        static AudioSystem& Instance() {
            static AudioSystem sys;
            return sys;
        }
        void init();
        void uninit();
        void update();

        bool load_audio_from_data(
            const std::filesystem::path& audio_folder_path,
            const std::filesystem::path& data_file_path,
            const Engine::Util::DataParser& parser,
            std::mutex& resources_mutex
        );
        bool load_audio_as_pcm(
            const std::filesystem::path& filePath,
            const std::string& UID,
            const std::string& group_UID,
            uint8_t MAX_INSTANCE
        );
        bool load_audio_as_stream(
            const std::filesystem::path& filePath,
            const std::string& UID,
            const std::string& group_UID,
            uint8_t MAX_INSTANCE
        );
        void add_sound_group(const std::string& UID);

        void play_audio_directly(const std::filesystem::path& path);
        Sound_Instance_ID play_audio_pcm(
            uint16_t sound_ID, bool loop, bool positioned = false,
            float x = 0.0f, float y = 0.0f, float z = 0.0f
        );
        Sound_Instance_ID play_audio_stream(
            uint16_t sound_ID, bool loop, bool positioned = false,
            float x = 0.0f, float y = 0.0f, float z = 0.0f
        );
        Sound_Instance_ID play_sound(
            uint16_t sound_ID, bool loop, bool positioned = false,
            float x = 0.0f, float y = 0.0f, float z = 0.0f
        );
        void stop_sound(uint16_t sound_id, uint8_t instance, uint8_t gen);
        bool sound_is_playing(uint16_t sound_id, uint8_t instance, uint8_t gen);

        bool pcm_sound_is_playing(uint8_t instance, uint8_t gen);
        void stop_pcm_sound(uint8_t instance, uint8_t gen);
        void free_pcm_sound(uint8_t instance);

        bool stream_sound_is_playing(uint8_t instance, uint8_t gen);
        void stop_stream_sound(uint8_t instance, uint8_t gen);
        void free_stream_sound(uint8_t instance);

        void set_listener_pos(float x, float y, float z);
        void set_listener_direction(float x, float y, float z);

        void set_master_volume(float volume);
        float get_master_volume();

        void set_group_volume(uint32_t ID, float volume);
        float get_group_volume(uint32_t ID);

        std::optional<uint32_t> get_id_with_uid(const std::string& UID) const;
    private:
        AudioSystem() = default;
        ~AudioSystem() = default;
        ma_engine audio_engine;

        std::vector<std::unique_ptr<AudioResource>> audio_pcm_database;
        std::vector<std::unique_ptr<AudioStreamResource>> audio_stream_database;
        std::vector<SoundEntry> sounds;
        std::vector<std::unique_ptr<SoundGroup>> sound_groups;

        std::vector<PoolSound> sounds_pool;
        std::vector<uint32_t> free_sounds;

        std::vector<PoolStreamSound> stream_sounds_pool;
        std::vector<uint32_t> free_stream_sounds;

        std::deque<std::string> sound_UID;
        std::deque<std::string> sound_group_UID;
        std::unordered_map<std::string_view, uint32_t> UID_to_ID;
        size_t total_audio_pcm_bytes_size = 0;
    };
}