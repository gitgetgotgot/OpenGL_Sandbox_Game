#pragma once
#include <Utility/Sprite.h>
#include <unordered_map>
#include <string>

class AnimationClip {
public:
	void add_sprite(uint32_t sprite_id);
	void set_frame_time(float time);
	float frame_time = 0.0f; //time between 2 frames
	float total_time = 0.0f; //total time of clip
	bool looping = false;
	std::vector<uint32_t> sprites; //sequence of clip sprites
};

class AnimationClipManager {
public:
	static AnimationClipManager* get_instance() {
		static AnimationClipManager mgr;
		return &mgr;
	}
	AnimationClip* get_clip(uint32_t id) {
		return &clips[id];
	}
	void add_clip() {
		clips.emplace_back();
	}
private:
	std::vector<AnimationClip> clips;
};

class Animator {
public:
	std::vector<uint32_t> clips;
};

class AnimatorManager {
public:
	static AnimatorManager* get_instance() {
		static AnimatorManager mgr;
		return &mgr;
	}
	Animator* get_animator(uint32_t id) {
		return &animators[id];
	}
	void add_animator() {
		animators.emplace_back();
	}
private:
	std::vector<Animator> animators;
};

class AnimatorController {
public:
	void set_animator(uint32_t id);
	void update();
	void play(uint32_t clip_id);
	uint32_t get_clip_id(uint32_t animator_clip_index);
	void stop();

	uint32_t animator_id = 0;
	Animator* animator_ptr = nullptr;
	AnimationClip* current_clip = nullptr;
	Sprite* current_sprite = nullptr;
	float current_frame_time = 0.0f;
	uint32_t current_sprite_index = 0;
	uint32_t max_sprite_index = 0;
	bool is_playing = false;
};