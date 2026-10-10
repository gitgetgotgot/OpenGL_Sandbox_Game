#include "Audio/AudioSystem.h"
#include <iostream>

void CoreAudio::AudioSystem::init() {
    ma_result res = ma_engine_init(nullptr, &audio_engine);
    if (res != MA_SUCCESS) {
        throw std::runtime_error("[AudioSystem] Audio Engine failed to initialize!");
    }

    sounds_pool.resize(AUDIO_MAX_SOUNDS);
    free_sounds.resize(AUDIO_MAX_SOUNDS);

    stream_sounds_pool.resize(AUDIO_STREAM_MAX_SOUNDS);
    free_stream_sounds.resize(AUDIO_STREAM_MAX_SOUNDS);

    static const float dummy_pcm[2] = { 0.0f, 0.0f };
    for (uint32_t i = 0; i < AUDIO_MAX_SOUNDS; ++i) {
        free_sounds[i] = i;
        PoolSound& sound = sounds_pool[i];
        ma_audio_buffer_config buf_config = ma_audio_buffer_config_init(ma_format_f32, 2, 1, dummy_pcm, NULL);
        ma_audio_buffer_init(&buf_config, &sound.audio_buffer);
    }
    for (uint32_t i = 0; i < AUDIO_STREAM_MAX_SOUNDS; ++i) {
        free_stream_sounds[i] = i;
    }

    audio_pcm_database.emplace_back();
    audio_stream_database.emplace_back();
    sound_groups.emplace_back();
}

void CoreAudio::AudioSystem::uninit() {
    for (auto& sound : sounds_pool) {
        if (sound.is_used) {
            ma_sound_stop(&sound.sound);
            ma_sound_uninit(&sound.sound);
        }
        ma_audio_buffer_uninit(&sound.audio_buffer);
    }
    for (auto& stream_sound : stream_sounds_pool) {
        if (stream_sound.is_used) {
            ma_sound_stop(&stream_sound.sound);
            ma_sound_uninit(&stream_sound.sound);
            ma_decoder_uninit(&stream_sound.decoder);
        }
    }
    for (auto& sound_group : sound_groups) {
        if (sound_group) {
            ma_sound_group_uninit(&sound_group->group);
        }
    }
    ma_engine_uninit(&audio_engine);
}

void CoreAudio::AudioSystem::update() {
    size_t size = sounds_pool.size();
    for (size_t i = 0; i < size; ++i) {
        PoolSound& sound = sounds_pool[i];
        if (sound.is_used && ma_sound_at_end(&sound.sound) == MA_TRUE) {
            free_pcm_sound(static_cast<uint8_t>(i));
        }
    }
    size = stream_sounds_pool.size();
    for (size_t i = 0; i < size; ++i) {
        PoolStreamSound& sound = stream_sounds_pool[i];
        if (sound.is_used && ma_sound_at_end(&sound.sound) == MA_TRUE) {
            free_stream_sound(static_cast<uint8_t>(i));
        }
    }
}

void CoreAudio::AudioSystem::load_audio_as_pcm(
    const std::filesystem::path& filePath,
    const std::string UID,
    const std::string group_UID,
    uint8_t MAX_INSTANCE
) {
    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, 48000); // Stereo - 2 output channels

    if (ma_decoder_init_file(filePath.string().data(), &config, &decoder) != MA_SUCCESS) {
        std::cerr << "[AudioSystem] File doesnt exist or currupted: " << filePath << std::endl;
        return;
    }
    ma_uint64 total_frames = 0;
    if (ma_decoder_get_length_in_pcm_frames(&decoder, &total_frames) != MA_SUCCESS) {
        std::cerr << "[AudioSystem] Failed to get length for: " << filePath << std::endl;
        return;
    }
    auto audio_res = std::make_unique<AudioResource>();
    audio_res->pcm_data.resize(total_frames * decoder.outputChannels);

    ma_uint64 frames_read = 0;
    if (ma_decoder_read_pcm_frames(&decoder, audio_res->pcm_data.data(), total_frames, &frames_read) != MA_SUCCESS) {
        std::cerr << "[AudioSystem] Failed to read PCM frames for: " << filePath << std::endl;
        return;
    }
    audio_res->frames_read = frames_read;
    audio_res->channels = decoder.outputChannels;
    audio_res->sample_rate = decoder.outputSampleRate;

    ma_decoder_uninit(&decoder);

    audio_UID.push_back(UID);
    std::string_view uid_view = audio_UID.back();
    UID_to_ID.emplace(uid_view, audio_pcm_database.size());
    auto& new_audio_res = audio_pcm_database.emplace_back(std::move(audio_res));

    std::cout << "[AudioSystem] Loaded audio PCM: <" << uid_view << "> (" <<
        (new_audio_res->pcm_data.size() * sizeof(float)) / 1024.0f << " KB)\n";
}

void CoreAudio::AudioSystem::load_audio_as_stream(
    const std::filesystem::path& filePath,
    const std::string UID,
    const std::string group_UID,
    uint8_t MAX_INSTANCE
) {

}

void CoreAudio::AudioSystem::add_sound_group(const std::string UID) {
    std::string_view uid_view = sound_group_UID.emplace_back(UID);
    UID_to_ID.emplace(uid_view, sound_groups.size());
    auto& new_group = sound_groups.emplace_back(std::make_unique<SoundGroup>());
    ma_sound_group_init(&audio_engine, 0, NULL, &new_group->group);
}

void CoreAudio::AudioSystem::play_audio_directly(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "[AudioSystem] Audio file doesn't exist: " << std::filesystem::absolute(path) << std::endl;
        return;
    }
    ma_engine_play_sound(&audio_engine, path.string().data(), nullptr);
}

CoreAudio::Sound_Instance_ID CoreAudio::AudioSystem::play_audio_pcm(
    const uint16_t& sound_ID, bool loop, bool positioned = false,
    float x = 0.0f, float y = 0.0f, float z = 0.0f
) {
    if (free_sounds.empty())
        return Sound_Instance_ID{ 0, 0 };

    AudioResource& audio_res = *audio_pcm_database[sound_ID];
    if (audio_res.current_instance_count == audio_res.MAX_INSTANCE_COUNT)
        return Sound_Instance_ID{ 0, 0 };

    uint32_t free_index = free_sounds.back();
    free_sounds.pop_back();
    PoolSound& sound_slot = sounds_pool[free_index];

    sound_slot.is_used = true;
    sound_slot.sound_id = sound_ID;

    if (sound_slot.generation == 255) sound_slot.generation = 1;
    else ++sound_slot.generation;

    //update ma_audio_buffer reference data
    sound_slot.audio_buffer.ref.pData = audio_res.pcm_data.data();
    sound_slot.audio_buffer.ref.sizeInFrames = audio_res.frames_read;
    sound_slot.audio_buffer.ref.cursor = 0;
    sound_slot.audio_buffer.ref.channels = audio_res.channels;
    sound_slot.audio_buffer.ref.format = ma_format_f32;

    ma_sound_init_from_data_source(&audio_engine, &sound_slot.audio_buffer, 0, &sound_groups[audio_res.sound_group_ID]->group, &sound_slot.sound);
    ma_sound_set_looping(&sound_slot.sound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(&sound_slot.sound);

    ++audio_res.current_instance_count;
    return Sound_Instance_ID{ static_cast<uint8_t>(free_index), sound_slot.generation };
}

CoreAudio::Sound_Instance_ID CoreAudio::AudioSystem::play_audio_stream(
    const uint16_t& sound_ID, bool loop, bool positioned = false,
    float x = 0.0f, float y = 0.0f, float z = 0.0f
) {
    if (free_stream_sounds.empty())
        return Sound_Instance_ID{ 0, 0 };

    AudioStreamResource& audio_res = *audio_stream_database[sound_ID];
    if (audio_res.current_instance_count == audio_res.MAX_INSTANCE_COUNT) {
        return Sound_Instance_ID{ 0, 0 };
    }

    uint32_t free_index = free_stream_sounds.back();
    free_stream_sounds.pop_back();

    PoolStreamSound& sound_slot = stream_sounds_pool[free_index];
    sound_slot.is_used = true;
    sound_slot.sound_id = sound_ID;

    if (sound_slot.generation == 255) sound_slot.generation = 1;
    else ++sound_slot.generation;

    ma_decoder_config decoder_config = ma_decoder_config_init(ma_format_f32, 0, 0);
    if (ma_decoder_init_file(audio_res.file_path.string().c_str(), &decoder_config, &sound_slot.decoder) != MA_SUCCESS) {
        sound_slot.is_used = false;
        free_stream_sounds.push_back(free_index);
        return Sound_Instance_ID{ 0, 0 };
    }

    ma_sound_init_from_data_source(&audio_engine, &sound_slot.decoder, 0, &sound_groups[audio_res.sound_group_ID]->group, &sound_slot.sound);
    ma_sound_set_looping(&sound_slot.sound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(&sound_slot.sound);

    ++audio_res.current_instance_count;

    return Sound_Instance_ID{ static_cast<uint8_t>(free_index), sound_slot.generation };
}

CoreAudio::Sound_Instance_ID CoreAudio::AudioSystem::play_sound(
    const uint16_t& sound_ID, bool loop, bool positioned = false,
    float x = 0.0f, float y = 0.0f, float z = 0.0f
) {
    SoundEntry& entry = sounds[sound_ID];
    if (entry.type == SOUND_PCM) return play_audio_pcm(entry.id, loop, positioned, x, y, z);
    else return play_audio_stream(entry.id, loop, positioned, x, y, z);
}

void CoreAudio::AudioSystem::stop_sound(const uint16_t& sound_id, const uint8_t& instance, const uint8_t& gen) {
    SoundEntry& entry = sounds[sound_id];
    if (entry.type == SOUND_PCM) stop_pcm_sound(instance, gen);
    else stop_stream_sound(instance, gen);
}

bool CoreAudio::AudioSystem::sound_is_playing(const uint16_t& sound_id, const uint8_t& instance, const uint8_t& gen) {
    SoundEntry& entry = sounds[sound_id];
    if (entry.type == SOUND_PCM) return pcm_sound_is_playing(instance, gen);
    else return stream_sound_is_playing(instance, gen);
}

bool CoreAudio::AudioSystem::pcm_sound_is_playing(const uint8_t& instance, const uint8_t& gen) {
    PoolSound& sound = sounds_pool[instance];
    return sound.is_used && gen == sound.generation && ma_sound_is_playing(&sound.sound);
}

void CoreAudio::AudioSystem::stop_pcm_sound(const uint8_t& instance, const uint8_t& gen) {
    PoolSound& sound = sounds_pool[instance];
    if (gen != sound.generation || !sound.is_used) return;
    ma_sound_stop(&sound.sound);
    free_pcm_sound(instance);
}

void CoreAudio::AudioSystem::free_pcm_sound(const uint8_t& instance) {
    PoolSound& sound = sounds_pool[instance];
    --audio_pcm_database[sound.sound_id]->current_instance_count;
    sound.is_used = false;
    ma_sound_uninit(&sound.sound);
    free_sounds.push_back(instance);
}

bool CoreAudio::AudioSystem::stream_sound_is_playing(const uint8_t& instance, const uint8_t& gen) {
    PoolStreamSound& sound = stream_sounds_pool[instance];
    return sound.is_used && gen == sound.generation && ma_sound_is_playing(&sound.sound);
}

void CoreAudio::AudioSystem::stop_stream_sound(const uint8_t& instance, const uint8_t& gen) {
    PoolStreamSound& sound = stream_sounds_pool[instance];
    if (gen != sound.generation || !sound.is_used) return;
    ma_sound_stop(&sound.sound);
    free_stream_sound(instance);
}

void CoreAudio::AudioSystem::free_stream_sound(const uint8_t& instance) {
    PoolStreamSound& sound = stream_sounds_pool[instance];
    if (!sound.is_used) return;
    --audio_stream_database[sound.sound_id]->current_instance_count;
    sound.is_used = false;
    ma_sound_uninit(&sound.sound);
    ma_decoder_uninit(&sound.decoder);
    free_stream_sounds.push_back(instance);
}

void CoreAudio::AudioSystem::set_listener_pos(float x, float y, float z) {
    ma_engine_listener_set_position(&audio_engine, 0, x, y, z);
}

void CoreAudio::AudioSystem::set_master_volume(float volume) {
    ma_engine_set_volume(&audio_engine, volume);
}
float CoreAudio::AudioSystem::get_master_volume() {
    ma_engine_get_volume(&audio_engine);
}

void CoreAudio::AudioSystem::set_group_volume(uint32_t ID, float volume) {
    ma_sound_group_set_volume(&sound_groups[ID]->group, volume);
}
float CoreAudio::AudioSystem::get_group_volume(uint32_t ID) {
    return ma_sound_group_get_volume(&sound_groups[ID]->group);
}

std::optional<uint32_t> CoreAudio::AudioSystem::get_id_with_uid(std::string_view UID) const {
    if (const auto it = UID_to_ID.find(UID); it != UID_to_ID.end())
        return it->second;
    else
        return std::nullopt;
}