#pragma once
#include <cstdint>
#include <string>

class AudioSource {
public:
	void setup_audio_source(uint32_t sound_ID);
	void setup_audio_source(std::string_view UID);
	void play(bool loop = false);
	void stop();
	bool is_playing() const;
private:
	uint32_t sound_ID = 0;
	uint8_t sound_instance_ID = 0;
	uint8_t sound_generation = 0;
};