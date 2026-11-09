#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace Engine::Util {
	class DataParser;
}

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
			const std::filesystem::path& data_file_path,
			const Engine::Util::DataParser& parser
		);
		AnimationClip* get_clip(uint32_t id);
		std::optional<uint32_t> get_clip_id(const std::string& uid);
		AnimationClip& add_clip(const std::string& uid);
	private:
		std::unordered_map<std::string, uint32_t> UID_to_ID;
		std::vector<AnimationClip> clips;
	};
}