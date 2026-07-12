#pragma once
#include <Rendering/Texture.h>

class GlyphData {
	//default values are for SPACE(32)
public:
	float U0 = 0.0f, V0 = 0.0f, U1 = 0.0f, V1 = 0.0f;
	float advance_normalized = 0.5f;
	float bearingX_normalized = 0.0f;
	float bearingY_normalized = 0.0f;
	float baselineY_normalized = 0.0f;
	float height_normalized = 0.0f;
	float width_normalized = 0.0f;
};

struct Font {
public:
	GlyphData glyphsData[95]; //[32, 126]
	float ascender;
	float descender;
};

constexpr char SDFFontAtlasesDirectory[] = "Resources/SDF_Fonts/";

class SDF_Font_Manager {
public:
	void load_main_sdf_font(const char* fontName);
	Font mainFont;
	Texture mainFontTexture;
};