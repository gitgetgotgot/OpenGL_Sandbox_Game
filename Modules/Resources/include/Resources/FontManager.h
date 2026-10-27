#pragma once
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <cstdint>

namespace Engine::Resource {
	struct Font {
	public:
		class GlyphData {
			//default values are for SPACE(32)
		public:
			float U0 = 0.0f, V0 = 0.0f, U1 = 0.0f, V1 = 0.0f;
			float width_normalized = 0.0f;
			float height_normalized = 0.0f;
			float advance_normalized = 0.5f;
			float bearingX_normalized = 0.0f;
			float bearingY_normalized = 0.0f;
		};
		GlyphData glyphsData[95]; //[32, 126]
		float ascender = 0.0f;
		float descender = 0.0f;
	};

	class FontManager {
	public:
		static FontManager& Instance() {
			static FontManager mgr;
			return mgr;
		}
		void load_font(std::filesystem::path font_folder_path, std::string source);
		std::optional<uint32_t> get_font_id(std::string font_UID);
		Font& get_font(uint32_t font_id);
	private:
		FontManager() {}
		~FontManager() {}
		std::unordered_map<std::string, uint32_t> UID_to_ID;
		std::vector<Font> fonts;
	};
}