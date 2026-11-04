#include "Entities/AnimatorComp.h"
#include <Utility/TimeManager.h>

void Engine::Entity::AnimatorComponent::set_animator(uint32_t id) {
	animator_ptr = Engine::Resource::AnimatorManager::Instance().get_animator(id);
	animator_id = id;
}

void Engine::Entity::AnimatorComponent::update() {
	if (!is_playing) return;

	current_frame_time += Engine::Util::TimeManager::deltaTime * speed;
	if (current_frame_time >= current_clip->frame_time) {
		current_frame_time = 0.0f;
		current_sprite_index++;
		if (current_sprite_index == max_sprite_index) { //stop or continue anim clip only after "frame time" of the last sprite
			if (current_clip->looping) {
				current_sprite_index = 0;
			}
			else {
				is_playing = false;
				return;
			}
		}
		current_sprite = &Engine::Resource::SpriteManager::Instance().get_sprite(current_clip->sprites[current_sprite_index]);
	}
}

void Engine::Entity::AnimatorComponent::play(uint32_t clip_id) {
	current_clip = Engine::Resource::AnimationClipManager::Instance().get_clip(clip_id);

	is_playing = true;
	max_sprite_index = current_clip->sprites.size();
	current_sprite_index = 0;
	current_frame_time = 0.0f;
	current_sprite = &Engine::Resource::SpriteManager::Instance().get_sprite(current_clip->sprites[current_sprite_index]);
}

uint32_t Engine::Entity::AnimatorComponent::get_clip_id(uint32_t animator_clip_index) {
	return animator_ptr->clips[animator_clip_index];
}

void Engine::Entity::AnimatorComponent::stop() {
	is_playing = false;
}