#include <vector>
#include <filesystem>
#include <miniaudio.h>

namespace CoreAudio {
    constexpr uint32_t AUDIO_MAX_SOUNDS = 64;
    constexpr uint32_t AUDIO_STREAM_MAX_SOUNDS = 8;

    struct SoundGroup {
        ma_sound_group group;
    };

    struct AudioResource {
        std::vector<float> pcm_data;
        ma_uint64 frames_read = 1;
        ma_uint32 sample_rate = 48000;
        uint32_t sound_group_ID = 0;
        ma_uint16 channels = 2;
        uint8_t current_instance_count = 0;
        uint8_t MAX_INSTANCE_COUNT = 8;
    };

    struct AudioStreamResource {
        std::filesystem::path file_path;
        uint32_t sound_group_ID = 0;
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

    struct PoolStreamSound {
        ma_sound sound;
        ma_decoder decoder;
        uint32_t sound_id = 0;
        uint8_t generation = 1;
        bool is_used = false;
    };

    struct Sound_Instance_ID {
        uint8_t instance_id = 0;
        uint8_t generation = 1;
    };

    enum SoundType : uint8_t { SOUND_PCM, SOUND_STREAM };

    struct SoundEntry {
        SoundType type = SoundType::SOUND_PCM;
        uint16_t id = 0;
    };
}