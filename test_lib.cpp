#include "src/Speaker.h"

class ModSpeaker : public BaseSpeaker {
	void speak() override {
		std::cout << "...This is Mod Speaker" << std::endl;
	}
};

class ModSpeakerFactory : public SpeakerFactory {
public:
	BaseSpeaker* create_speaker() override {
		return new ModSpeaker();
	}
};

EXPORT SpeakerFactory* GET_SPEAKER_FACTORY() {
	return new ModSpeakerFactory();
}