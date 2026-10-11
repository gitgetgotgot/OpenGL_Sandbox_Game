#pragma once
#include <cstdint>
#include <string>

namespace CoreAudio {
	class AudioSource {
	public:
		AudioSource() = default;
		AudioSource(const AudioSource&);
		AudioSource(AudioSource&&);
		AudioSource& operator=(const AudioSource&);
		AudioSource& operator=(AudioSource&&);

		void setup_source(uint16_t sound_ID);
		void setup_source(std::string_view sound_UID);
		void play(bool loop = false);
		void stop();
		bool is_playing() const;
		void set_volume(float volume);
		void set_pitch(float pitch);
	private:
		float volume = 1.0f;
		float pitch = 1.0f;
		uint16_t sound_ID = 0;
		uint8_t sound_instance_ID = 0;
		uint8_t sound_generation = 0;
	};
}