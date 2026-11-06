#pragma once
#include <Base/Types.h>
#include <vector>
#include <optional>
#include <unordered_map>
#include <string>
#include <Utility/DataParser.h>

namespace Engine::Resource {
	class Animator {
	public:
		std::vector<uint32_t> clips;
	};

	class AnimatorManager {
	public:
		static AnimatorManager& Instance() {
			static AnimatorManager mgr;
			return mgr;
		}
		void ClearData();
		bool load_animators_from_data(
			std::filesystem::path& data_file_path,
			Engine::Util::DataParser& parser
		);
		Animator* get_animator(uint32_t id);
		std::optional<uint32_t> get_animator_id(const std::string& clip_uid);
		Animator& add_animator(const std::string& uid);
	private:
		std::unordered_map<std::string, uint32_t> animator_UID_to_ID;
		std::vector<Animator> animators;
	};
}