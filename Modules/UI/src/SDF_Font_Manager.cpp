#include <UI/SDF_Font_Manager.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

void SDF_Font_Manager::load_main_sdf_font(const char* fontName) {
	//Font setup

	uint32_t glyph_atlas_size = 0;

	//load atlas bitmap
	std::string file_path;
	std::string filename = std::filesystem::path(fontName).filename().string();

	file_path = SDFFontAtlasesDirectory + filename + ".atlas";
	std::ifstream atlas_file(file_path, std::ios::binary);

	atlas_file.read(reinterpret_cast<char*>(&glyph_atlas_size), sizeof(glyph_atlas_size));

	std::vector<unsigned char> sdf(glyph_atlas_size * glyph_atlas_size, 0);
	atlas_file.read(reinterpret_cast<char*>(sdf.data()), sdf.size());
	atlas_file.close();

	//load atlas meta
	file_path = SDFFontAtlasesDirectory + filename + ".meta";
	std::ifstream meta_file(file_path, std::ios::binary);

	uint32_t glyphs_count = 0;
	meta_file.read(reinterpret_cast<char*>(&glyphs_count), sizeof(glyphs_count));
	meta_file.read(reinterpret_cast<char*>(&mainFont.ascender), sizeof(mainFont.ascender));
	meta_file.read(reinterpret_cast<char*>(&mainFont.descender), sizeof(mainFont.descender));

	for (auto& glyph : mainFont.glyphsData) {
		meta_file.read(reinterpret_cast<char*>(&glyph), sizeof(GlyphData));
	}
	meta_file.close();

	//uint32_t tex_id = TextureManager::get_Instance()->create_texture_from_data(sdf.data(), glyph_atlas_size, glyph_atlas_size, 1, false)->texture_id;
	mainFontTexture.bind(1);
	mainFontTexture.load_R8_texture_from_buffer(sdf.data(), glyph_atlas_size, glyph_atlas_size, false);
	mainFontTexture.bind(1);

	std::cout << "SDF Font loaded successfully" << std::endl;
	std::cout << "Font ascender = " << mainFont.ascender << std::endl;
	std::cout << "Font descender = " << mainFont.descender << std::endl;
}
