#pragma once
#include <cstdint>
#include <string>

namespace CoreAudio {
	class AudioGroupMixer {
	public:
		void setup_mixer(uint32_t group_ID);
		void setup_mixer(std::string_view group_UID);
		void set_volume(float volume) const;
		float get_volume() const;
	private:
		uint32_t group_ID = 0;
	};
}