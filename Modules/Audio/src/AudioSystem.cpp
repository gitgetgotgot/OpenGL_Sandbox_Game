#include "Audio/AudioSystem.h"
#include <iostream>

void AudioSystem::init() {
	ma_result res = ma_engine_init(nullptr, &audio_engine);
	if (res != MA_SUCCESS) {
		throw std::runtime_error("[AudioSystem] Audio Engine failed to initialize!");
	}

	sounds_pool.resize(AUDIO_MAX_SOUNDS);
    free_sounds.resize(AUDIO_MAX_SOUNDS);

    static const float dummy_pcm[2] = { 0.0f, 0.0f };
    for (uint32_t i = 0; i < AUDIO_MAX_SOUNDS; ++i) {
        free_sounds[i] = i;

        PoolSound& sound = sounds_pool[i];
        ma_audio_buffer_config buf_config = ma_audio_buffer_config_init(ma_format_f32, 2, 1, dummy_pcm, NULL);
        ma_audio_buffer_init(&buf_config, &sound.audio_buffer);
        ma_sound_init_from_data_source(&audio_engine, &sound.audio_buffer, 0, NULL, &sound.sound);
    }

    auto& empty_audio_resourse = audio_pcm_database.emplace_back(std::make_unique<AudioResource>());
    empty_audio_resourse->pcm_data.resize(2);
    std::memcpy(empty_audio_resourse->pcm_data.data(), dummy_pcm, sizeof(float) * 2);
}

void AudioSystem::uninit() {
    for (auto& sound : sounds_pool) {
        if (sound.is_used) ma_sound_stop(&sound.sound);
        ma_sound_uninit(&sound.sound);
        ma_audio_buffer_uninit(&sound.audio_buffer);
    }
	ma_engine_uninit(&audio_engine);
}

void AudioSystem::update() {
    size_t size = sounds_pool.size();
    for (size_t i = 0; i < size; ++i) {
        PoolSound& sound = sounds_pool[i];
        if (sound.is_used && ma_sound_at_end(&sound.sound) == MA_TRUE) {
            free_sound(static_cast<uint8_t>(i));
        }
    }
}

void AudioSystem::play_audio_directly(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		std::cerr << "[AudioSystem] Audio file doesn't exist: " << std::filesystem::absolute(path) << std::endl;
		return;
	}
	ma_engine_play_sound(&audio_engine, path.string().data(), nullptr);
}

void AudioSystem::play_audio_stream(const std::filesystem::path& path) {

}

void AudioSystem::play_audio_stream(uint32_t audio_ID) {

}

Sound_Instance_ID AudioSystem::play_audio_pcm(uint32_t audio_ID, bool loop) {
    if (free_sounds.empty())
        return Sound_Instance_ID{ 0, 0 };

    AudioResource& audio_res = *audio_pcm_database[audio_ID];
    if (audio_res.current_instance_count == audio_res.MAX_INSTANCE_COUNT)
        return Sound_Instance_ID{ 0, 0 };

    uint32_t free_index = free_sounds.back();
    free_sounds.pop_back();
    PoolSound& sound_inst = sounds_pool[free_index];

    sound_inst.is_used = true;
    sound_inst.sound_id = audio_ID;
    ++audio_res.current_instance_count;

    if (sound_inst.generation == 255) sound_inst.generation = 1;
    else ++sound_inst.generation;

    //update ma_audio_buffer reference data
    sound_inst.audio_buffer.ref.pData = audio_res.pcm_data.data();
    sound_inst.audio_buffer.ref.sizeInFrames = audio_res.frames_read;
    sound_inst.audio_buffer.ref.cursor = 0;
    sound_inst.audio_buffer.ref.channels = audio_res.channels;
    sound_inst.audio_buffer.ref.format = ma_format_f32;

    ma_sound_set_looping(&sound_inst.sound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(&sound_inst.sound);
    
    return Sound_Instance_ID{ static_cast<uint8_t>(free_index), sound_inst.generation };
}

bool AudioSystem::sound_is_playing(uint8_t instance, uint8_t gen) {
    PoolSound& sound = sounds_pool[instance];
    return gen == sound.generation && ma_sound_is_playing(&sound.sound);
}

void AudioSystem::stop_sound(uint8_t instance, uint8_t gen) {
    PoolSound& sound = sounds_pool[instance];

    if (gen != sound.generation || !sound.is_used) return;

    ma_sound_stop(&sound.sound);
    free_sound(instance);
}

void AudioSystem::free_sound(uint8_t instance) {
    PoolSound& sound = sounds_pool[instance];

    --audio_pcm_database[sound.sound_id]->current_instance_count;
    sound.is_used = false;

    ma_sound_seek_to_pcm_frame(&sound.sound, 0);
    
    free_sounds.push_back(instance);
}

void AudioSystem::load_audio_as_pcm(const std::filesystem::path& filePath, std::string UID) {
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

void AudioSystem::load_audio_as_stream(const std::filesystem::path& filePath) {

}

void AudioSystem::set_global_volume(float volume) {
	ma_engine_set_volume(&audio_engine, volume);
}

std::optional<uint32_t> AudioSystem::get_sound_id(std::string_view UID) const {
    if (const auto it = UID_to_ID.find(UID); it != UID_to_ID.end())
        return it->second;
    else
        return std::nullopt;
}