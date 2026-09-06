#include <Utility/Animation.h>
#include <Utility/TimeManager.h>

void CoreAnimation::AnimationClip::add_sprite(uint32_t sprite_id) {
	sprites.emplace_back(sprite_id);
}

void CoreAnimation::AnimationClip::set_frame_time(float time) {
	frame_time = time;
	total_time = float(sprites.size()) * frame_time;
}

void CoreAnimation::AnimationClipManager::ClearData() {
	clips = {};
	clip_UID_to_ID = {};
}

CoreAnimation::AnimationClip* CoreAnimation::AnimationClipManager::get_clip(uint32_t id) {
	return &clips[id];
}

std::optional<uint32_t> CoreAnimation::AnimationClipManager::get_clip_id(std::string& clip_uid) {
	auto it = clip_UID_to_ID.find(clip_uid);
	if (it == clip_UID_to_ID.end()) return std::nullopt;
	else return it->second;
}

CoreAnimation::AnimationClip& CoreAnimation::AnimationClipManager::add_clip(std::string uid) {
	clip_UID_to_ID.emplace(uid, clips.size());
	return clips.emplace_back();
}

void CoreAnimation::AnimatorManager::ClearData() {
	animators = {};
	animator_UID_to_ID = {};
}

CoreAnimation::Animator* CoreAnimation::AnimatorManager::get_animator(uint32_t id) {
	return &animators[id];
}

std::optional<uint32_t> CoreAnimation::AnimatorManager::get_animator_id(std::string& clip_uid) {
	auto it = animator_UID_to_ID.find(clip_uid);
	if (it == animator_UID_to_ID.end()) return std::nullopt;
	else return it->second;
}

CoreAnimation::Animator& CoreAnimation::AnimatorManager::add_animator(std::string uid) {
	animator_UID_to_ID.emplace(uid, animators.size());
	return animators.emplace_back();
}

void CoreAnimation::AnimatorController::set_animator(uint32_t id) {
	animator_ptr = AnimatorManager::get_instance().get_animator(id);
	animator_id = id;
}

void CoreAnimation::AnimatorController::update() {
	if (!is_playing) return;

	current_frame_time += TimeManager::deltaTime * speed;
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
		current_sprite = &CoreResource::SpriteManager::get_instance().get_sprite(current_clip->sprites[current_sprite_index]);
	}
}

void CoreAnimation::AnimatorController::play(uint32_t clip_id) {
	current_clip = AnimationClipManager::get_instance().get_clip(clip_id);

	is_playing = true;
	max_sprite_index = current_clip->sprites.size();
	current_sprite_index = 0;
	current_frame_time = 0.0f;
	current_sprite = &CoreResource::SpriteManager::get_instance().get_sprite(current_clip->sprites[current_sprite_index]);
}

uint32_t CoreAnimation::AnimatorController::get_clip_id(uint32_t animator_clip_index) {
	return animator_ptr->clips[animator_clip_index];
}

void CoreAnimation::AnimatorController::stop() {
	is_playing = false;
}
