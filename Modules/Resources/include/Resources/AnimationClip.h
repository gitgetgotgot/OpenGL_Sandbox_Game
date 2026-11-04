#pragma once
#include "Resources/Sprite.h"
#include <unordered_map>
#include <string>

namespace Engine::Resource {
	class AnimationClip {
	public:
		void add_sprite(uint32_t sprite_id);
		void set_frame_time(float time);
	public:
		float frame_time = 0.0f; //time between 2 frames
		float total_time = 0.0f; //total time of clip
		bool looping = false;
		std::vector<uint32_t> sprites; //sequence of clip sprites
	};

	class AnimationClipManager {
	public:
		static AnimationClipManager& Instance() {
			static AnimationClipManager mgr;
			return mgr;
		}
		void ClearData();
		bool load_clips_from_data(
			std::filesystem::path& data_file_path,
			Engine::Util::DataParser& parser
		);
		AnimationClip* get_clip(uint32_t id);
		std::optional<uint32_t> get_clip_id(const std::string& clip_uid);
		AnimationClip& add_clip(const std::string& uid);
	private:
		std::unordered_map<std::string, uint32_t> clip_UID_to_ID;
		std::vector<AnimationClip> clips;
	};
}