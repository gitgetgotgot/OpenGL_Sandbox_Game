#include "Audio/AudioSource.h"
#include "Audio/AudioSystem.h"

void AudioSource::setup_audio_source(uint32_t sound_ID) {
	this->sound_ID = sound_ID;
}

void AudioSource::setup_audio_source(std::string_view UID) {
	auto result = AudioSystem::Instance().get_sound_id(UID);
	if (result.has_value())
		this->sound_ID = result.value();
	else
		this->sound_ID = 0;
}

void AudioSource::play(bool loop) {
	if (sound_ID) {
		stop();
		auto result = AudioSystem::Instance().play_audio_pcm(sound_ID, loop);
		sound_instance_ID = result.instance_id;
		sound_generation = result.generation;
	}
}

void AudioSource::stop() {
	if (sound_ID && sound_generation) {
		AudioSystem::Instance().stop_sound(sound_instance_ID, sound_generation);
		sound_generation = 0;
	}
}

bool AudioSource::is_playing() const {
	if (sound_ID && sound_generation)
		return AudioSystem::Instance().sound_is_playing(sound_instance_ID, sound_generation);
	else
		return false;
}