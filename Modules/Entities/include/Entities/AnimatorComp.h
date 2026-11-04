#pragma once
#include <Resources/Animator.h>
#include <Resources/AnimationClip.h>

namespace Engine::Entity {
	class AnimatorComponent {
	public:
		void set_animator(uint32_t id);
		void update();
		void play(uint32_t clip_id);
		uint32_t get_clip_id(uint32_t animator_clip_index);
		void stop();
	public:
		uint32_t animator_id = 0;
		Engine::Resource::Animator* animator_ptr = nullptr;
		Engine::Resource::AnimationClip* current_clip = nullptr;
		Engine::Resource::Sprite* current_sprite = nullptr;
		float current_frame_time = 0.0f;
		uint32_t current_sprite_index = 0;
		uint32_t max_sprite_index = 0;
		bool is_playing = false;
		float speed = 1.0f;
	};
}