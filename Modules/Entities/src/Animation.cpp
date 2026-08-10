#include <Entities/Animation.h>
#include <Utility/TimeManager.h>

void AnimationClip::add_sprite(uint32_t sprite_id) {
	sprites.emplace_back(sprite_id);
}

void AnimationClip::set_frame_time(float time) {
	frame_time = time;
	total_time = float(sprites.size()) * frame_time;
}

void AnimatorController::set_animator(uint32_t id) {
	animator_ptr = AnimatorManager::get_instance()->get_animator(id);
	animator_id = id;
}

void AnimatorController::update() {
	if (!is_playing) return;

	current_frame_time += TimeManager::deltaTime;
	if (current_frame_time >= current_clip->frame_time) {
		current_frame_time = 0.0f;
		current_sprite_index++;
		if (current_sprite_index > max_sprite_index) { //stop or continue anim clip only after "frame time" of the last sprite
			if (current_clip->looping) {
				current_sprite_index = 0;
			}
			else {
				is_playing = false;
				return;
			}
		}
		current_sprite = &SpriteManager::get_instance()->get_sprite(current_clip->sprites[current_sprite_index]);
	}
}

void AnimatorController::play(uint32_t clip_id) {
	current_clip = AnimationClipManager::get_instance()->get_clip(clip_id);

	is_playing = true;
	max_sprite_index = current_clip->sprites.size();
	current_sprite_index = 0;
	current_frame_time = 0.0f;
	current_sprite = &SpriteManager::get_instance()->get_sprite(current_clip->sprites[current_sprite_index]);
}

uint32_t AnimatorController::get_clip_id(uint32_t animator_clip_index) {
	return animator_ptr->clips[animator_clip_index];
}

void AnimatorController::stop() {
	is_playing = false;
}
