#pragma once
#include <filesystem>
#include <string>

const std::string input_fonts_folder = "InputFonts";
const std::string output_fonts_folder = "OutputFonts";

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

class FontGenerator {
public:
	static bool generate_truetype_sdf_font(std::filesystem::path& fontPath, bool write_atlas_to_png);
private:
	FontGenerator() = default;
	~FontGenerator() = default;
};