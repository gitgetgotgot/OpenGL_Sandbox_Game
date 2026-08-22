#pragma once
#include <iostream>

#define EXPORT extern "C" __declspec(dllexport)

class BaseSpeaker {
public:
	BaseSpeaker() {}
	~BaseSpeaker() {}
	virtual void speak() = 0;
};

class CoreSpeaker : public BaseSpeaker {
	void speak() override {
		std::cout << "...This is Core Speaker" << std::endl;
	}
};

class SpeakerFactory {
public:
	SpeakerFactory() {}
	~SpeakerFactory() {}
	virtual BaseSpeaker* create_speaker() = 0;
};

class CoreSpeakerFactory : public SpeakerFactory {
public:
	BaseSpeaker* create_speaker() override {
		return new CoreSpeaker();
	}
};