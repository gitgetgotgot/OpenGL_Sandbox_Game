#include "Audio/AudioSource.h"
#include "Audio/AudioSystem.h"

CoreAudio::AudioSource::AudioSource(const AudioSource& other) {
	this->sound_ID = other.sound_ID;
	this->volume = other.volume;
	this->pitch = other.pitch;
}

CoreAudio::AudioSource::AudioSource(AudioSource&& other) {
	this->sound_ID = other.sound_ID; other.sound_ID = 0;
	this->volume = other.volume;
	this->pitch = other.pitch;
	this->sound_instance_ID = other.sound_instance_ID; other.sound_instance_ID = 0;
	this->sound_generation = other.sound_generation; other.sound_generation = 0;
}

CoreAudio::AudioSource& CoreAudio::AudioSource::operator=(const AudioSource& other) {
	if (this != &other) {
		this->sound_ID = other.sound_ID;
		this->volume = other.volume;
		this->pitch = other.pitch;
	}
	return *this;
}

CoreAudio::AudioSource& CoreAudio::AudioSource::operator=(AudioSource&& other) {
	if (this != &other) {
		this->sound_ID = other.sound_ID; other.sound_ID = 0;
		this->volume = other.volume;
		this->pitch = other.pitch;
		this->sound_instance_ID = other.sound_instance_ID; other.sound_instance_ID = 0;
		this->sound_generation = other.sound_generation; other.sound_generation = 0;
	}
	return *this;
}

void CoreAudio::AudioSource::setup_source(uint16_t sound_ID) {
	this->sound_ID = sound_ID;
}

void CoreAudio::AudioSource::setup_source(std::string_view sound_UID) {
	auto result = AudioSystem::Instance().get_id_with_uid(sound_UID);
	if (result.has_value())
		this->sound_ID = result.value();
	else
		this->sound_ID = 0;
}

void CoreAudio::AudioSource::play(bool loop) {
	if (sound_ID) {
		stop();
		auto result = AudioSystem::Instance().play_sound(sound_ID, loop);
		sound_instance_ID = result.instance_id;
		sound_generation = result.generation;
	}
}

void CoreAudio::AudioSource::stop() {
	if (sound_ID && sound_generation) {
		AudioSystem::Instance().stop_sound(sound_instance_ID, sound_generation);
		sound_generation = 0;
	}
}

bool CoreAudio::AudioSource::is_playing() const {
	if (sound_ID && sound_generation)
		return AudioSystem::Instance().sound_is_playing(sound_instance_ID, sound_generation);
	else
		return false;
}

void CoreAudio::AudioSource::set_volume(float volume) {
	this->volume = volume;

}

void CoreAudio::AudioSource::set_pitch(float pitch) {
	this->pitch = pitch;

}