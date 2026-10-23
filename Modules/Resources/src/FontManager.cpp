#include "Resources/FontManager.h"
#include "Resources/TextureArrayMgr.h"
#include <fstream>

void CoreResource::FontManager::load_font(std::filesystem::path font_folder_path, std::string source) {
	std::filesystem::path file_path;
	std::string file_name;
	std::string font_name = font_folder_path.filename().string();

	//Font setup
	Font& new_font = fonts.emplace_back();
	uint32_t glyph_atlas_size = 0;

	//load atlas bitmap
	file_name = font_name + ".atlas";
	file_path = font_folder_path / file_name;
	if (!std::filesystem::exists(file_path)) {
		fonts.pop_back();
		throw std::runtime_error("<" + file_name + "> not found in " + font_folder_path.string());
	}
	std::ifstream atlas_file(file_path, std::ios::binary);

	atlas_file.read(reinterpret_cast<char*>(&glyph_atlas_size), sizeof(glyph_atlas_size));
	std::vector<unsigned char> sdf(glyph_atlas_size * glyph_atlas_size, 0);
	atlas_file.read(reinterpret_cast<char*>(sdf.data()), sdf.size());
	atlas_file.close();

	//load atlas meta
	file_name = font_name + ".meta";
	file_path = font_folder_path / file_name;
	if (!std::filesystem::exists(file_path)) {
		fonts.pop_back();
		throw std::runtime_error("<" + file_name + "> not found in " + font_folder_path.string());
	}
	std::ifstream meta_file(file_path, std::ios::binary);

	uint32_t glyphs_count = 0;
	meta_file.read(reinterpret_cast<char*>(&glyphs_count), sizeof(glyphs_count));
	meta_file.read(reinterpret_cast<char*>(&new_font.ascender), sizeof(new_font.ascender));
	meta_file.read(reinterpret_cast<char*>(&new_font.descender), sizeof(new_font.descender));

	for (auto& glyph : new_font.glyphsData) {
		meta_file.read(reinterpret_cast<char*>(&glyph), sizeof(Font::GlyphData));
	}
	meta_file.close();

	std::string font_UID = source + ":" + font_name;
	TextureArrayManager::Instance().load_font_texture(sdf.data(), glyph_atlas_size, glyph_atlas_size, 1, font_UID);
}

std::optional<uint32_t> CoreResource::FontManager::get_font_id(std::string font_UID) {
	if (auto it = UID_to_ID.find(font_UID); it != UID_to_ID.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}

CoreResource::Font& CoreResource::FontManager::get_font(uint32_t font_id) {
	return fonts[font_id];
}