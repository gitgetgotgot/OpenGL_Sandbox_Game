#include "Resources/Animator.h"
#include "Resources/AnimationClip.h"
#include <Utility/Logger.h>

void Engine::Resource::AnimatorManager::ClearData() {
	animators = {};
	animator_UID_to_ID = {};
}

bool Engine::Resource::AnimatorManager::load_animators_from_data(
	std::filesystem::path& data_file_path,
	Engine::Util::DataParser& parser
) {
	using DP = Engine::Util::DataParser;
	for (const auto& j : parser.node_root) {
		std::string UID;
		std::vector<std::string> clips;
		std::vector<uint32_t> clips_id_vec;
		bool ok = true;

		ok &= DP::get_json_value<std::string, DP::JsonValue::V_STRING>(UID, j, "UID");
		ok &= DP::get_json_array<std::vector<std::string>, DP::JsonValue::V_STRING>(clips, j, "clips");
		if (!ok) {
			Engine::Util::Logger::Log("Invalid syntax for Animator in <" + data_file_path.string() + ">!");
			return false;
		}
		clips_id_vec.reserve(clips.size());
		for (const auto& clip_UID : clips) {
			auto clip_global_id = Engine::Resource::AnimationClipManager::Instance().get_clip_id(clip_UID);
			if (!clip_global_id) {
				Engine::Util::Logger::Log("Couldn't find referenced resource <" + clip_UID + "> for Animator <" + UID + "> in <" + data_file_path.string() + ">!");
				return false;
			}
			clips_id_vec.push_back(clip_global_id.value());
		}
		Animator& animator = add_animator(UID);
		animator.clips = std::move(clips_id_vec);
	}
	return true;
}

Engine::Resource::Animator* Engine::Resource::AnimatorManager::get_animator(uint32_t id) {
	return &animators[id];
}

std::optional<uint32_t> Engine::Resource::AnimatorManager::get_animator_id(const std::string& clip_uid) {
	auto it = animator_UID_to_ID.find(clip_uid);
	if (it == animator_UID_to_ID.end()) return std::nullopt;
	else return it->second;
}

Engine::Resource::Animator& Engine::Resource::AnimatorManager::add_animator(const std::string& uid) {
	animator_UID_to_ID.emplace(uid, animators.size());
	return animators.emplace_back();
}