#include "Resources/AnimationClip.h"
#include "Resources/Sprite.h"
#include <Utility/TimeManager.h>
#include <Utility/Logger.h>

void Engine::Resource::AnimationClip::add_sprite(uint32_t sprite_id) {
	sprites.emplace_back(sprite_id);
}

void Engine::Resource::AnimationClip::set_frame_time(float time) {
	frame_time = time;
	total_time = float(sprites.size()) * frame_time;
}

void Engine::Resource::AnimationClipManager::ClearData() {
	clips = {};
	clip_UID_to_ID = {};
}

bool Engine::Resource::AnimationClipManager::load_clips_from_data(
	std::filesystem::path& data_file_path,
	Engine::Util::DataParser& parser
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		std::string UID;
		bool loop;
		float frame_time;
		std::vector<std::string> sprites;
		std::vector<uint32_t> sprites_id_vec;
		bool ok = true;

		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(UID, j, "UID");
		ok &= DP::get_json_value<bool, DP::JsonValue::V_BOOL>(loop, j, "loop");
		ok &= DP::get_json_value<float, DP::JsonValue::V_FLOAT>(frame_time, j, "frame_time");
		ok &= DP::get_json_array<std::vector<std::string>, DP::JsonValue::V_STRING>(sprites, j, "sprites");
		if (!ok) {
			Engine::Util::Logger::Log("Invalid syntax for Animation Clip in <" + data_file_path.string() + ">!");
			return false;
		}
		sprites_id_vec.reserve(sprites.size());
		for (const auto& sprite_UID : sprites) {
			auto sprite_global_id = Engine::Resource::SpriteManager::Instance().get_sprite_id(sprite_UID);
			if (!sprite_global_id) {
				Engine::Util::Logger::Log("Couldn't find referenced resource <" + sprite_UID + "> for Animation Clip <" + UID + "> in <" + data_file_path.string() + ">!");
				return false;
			}
			sprites_id_vec.push_back(sprite_global_id.value());
		}
		AnimationClip& clip = add_clip(UID);
		clip.looping = loop;
		clip.set_frame_time(frame_time); // set frame time and count total clip time
		clip.sprites = std::move(sprites_id_vec);
	}
	return true;
}

Engine::Resource::AnimationClip* Engine::Resource::AnimationClipManager::get_clip(uint32_t id) {
	return &clips[id];
}

std::optional<uint32_t> Engine::Resource::AnimationClipManager::get_clip_id(const std::string& clip_uid) {
	auto it = clip_UID_to_ID.find(clip_uid);
	if (it == clip_UID_to_ID.end()) return std::nullopt;
	else return it->second;
}

Engine::Resource::AnimationClip& Engine::Resource::AnimationClipManager::add_clip(const std::string& uid) {
	clip_UID_to_ID.emplace(uid, clips.size());
	return clips.emplace_back();
}