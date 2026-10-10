#include "Audio/AudioGroupMixer.h"
#include "Audio/AudioSystem.h"

void CoreAudio::AudioGroupMixer::setup_mixer(uint32_t group_ID) {
	this->group_ID = group_ID;
}

void CoreAudio::AudioGroupMixer::setup_mixer(std::string_view group_UID) {
	auto result = AudioSystem::Instance().get_id_with_uid(group_UID);
	if (result.has_value())
		this->group_ID = result.value();
	else
		this->group_ID = 0;
}

void CoreAudio::AudioGroupMixer::set_volume(float volume) const {
	if (group_ID) AudioSystem::Instance().set_group_volume(group_ID, std::clamp(volume, 0.0f, 2.0f));
}

float CoreAudio::AudioGroupMixer::get_volume() const {
	if (group_ID)
		return AudioSystem::Instance().get_group_volume(group_ID);
	else
		return 0.0f;
}