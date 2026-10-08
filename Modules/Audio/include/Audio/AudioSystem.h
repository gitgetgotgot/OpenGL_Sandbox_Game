#pragma once
#include <vector>
#include <string>

class AudioSystem {
public:
    static AudioSystem& Instance() {
        static AudioSystem sys;
        return sys;
    }
    void init();
    void uninit();
    void load_Sound(const std::string& filePath);
    void play_Sound(int sound_id);
    void load_Music(const std::string& filePath);
    void play_Music(int music_id, bool loop);
    void stop_Music();
    void set_Music_Volume(int volume);
private:
    AudioSystem() = default;
    ~AudioSystem() = default;

};