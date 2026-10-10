#pragma once
#include <deque>
#include <optional>
#include <memory>
#include <unordered_map>
#include "Audio/AudioData.h"

namespace CoreAudio {
    class AudioSystem {
    public:
        static AudioSystem& Instance() {
            static AudioSystem sys;
            return sys;
        }
        void init();
        void uninit();
        void update();

        void load_audio_as_pcm(
            const std::filesystem::path& filePath,
            const std::string UID,
            const std::string group_UID,
            uint8_t MAX_INSTANCE
        );
        void load_audio_as_stream(
            const std::filesystem::path& filePath,
            const std::string UID,
            const std::string group_UID,
            uint8_t MAX_INSTANCE
        );
        void add_sound_group(const std::string UID);

        void play_audio_directly(const std::filesystem::path& path);
        Sound_Instance_ID play_audio_pcm(
            const uint16_t& sound_ID, bool loop, bool positioned = false,
            float x = 0.0f, float y = 0.0f, float z = 0.0f
        );
        Sound_Instance_ID play_audio_stream(
            const uint16_t& sound_ID, bool loop, bool positioned = false,
            float x = 0.0f, float y = 0.0f, float z = 0.0f
        );
        Sound_Instance_ID play_sound(
            const uint16_t& sound_ID, bool loop, bool positioned = false,
            float x = 0.0f, float y = 0.0f, float z = 0.0f
        );
        void stop_sound(const uint16_t& sound_id, const uint8_t& instance, const uint8_t& gen);
        bool sound_is_playing(const uint16_t& sound_id, const uint8_t& instance, const uint8_t& gen);

        bool pcm_sound_is_playing(const uint8_t& instance, const uint8_t& gen);
        void stop_pcm_sound(const uint8_t& instance, const uint8_t& gen);
        void free_pcm_sound(const uint8_t& instance);

        bool stream_sound_is_playing(const uint8_t& instance, const uint8_t& gen);
        void stop_stream_sound(const uint8_t& instance, const uint8_t& gen);
        void free_stream_sound(const uint8_t& instance);

        void set_listener_pos(float x, float y, float z);

        void set_master_volume(float volume);
        float get_master_volume();

        void set_group_volume(uint32_t ID, float volume);
        float get_group_volume(uint32_t ID);

        std::optional<uint32_t> get_id_with_uid(std::string_view UID) const;
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
    };
}