#include "FontGenerator.h"
#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

struct Point {
	float x, y;
};

struct LineSegment {
	Point a, b;
};

struct QuadraticCurve {
	Point p0, c, p2;
};

struct GlyphOutlineContext {
	std::vector<LineSegment> lines;
	std::vector<QuadraticCurve> curves;
	Point current_point{ 0, 0 };
};

// 1. Accurate distance from Point to Line
inline float distance_to_line(Point p, Point a, Point b) {
	float ab_x = b.x - a.x;
	float ab_y = b.y - a.y;
	float ap_x = p.x - a.x;
	float ap_y = p.y - a.y;

	float ab_len_sq = ab_x * ab_x + ab_y * ab_y;
	if (ab_len_sq == 0.0f) return std::hypot(p.x - a.x, p.y - a.y);

	float t = std::clamp((ap_x * ab_x + ap_y * ab_y) / ab_len_sq, 0.0f, 1.0f);
	float near_x = a.x + t * ab_x;
	float near_y = a.y + t * ab_y;

	return std::hypot(p.x - near_x, p.y - near_y);
}

// 2. Accurate distance from Point to quadratic Bezier curve
inline float distance_to_quadratic_bezier(Point p, Point p0, Point c, Point p2) {
	Point a = { p0.x - 2.0f * c.x + p2.x, p0.y - 2.0f * c.y + p2.y };
	Point b_coef = { 2.0f * (c.x - p0.x), 2.0f * (c.y - p0.y) };
	Point s = { p0.x - p.x, p0.y - p.y };

	// correct scalar products for cubic equation of the derivative f'(t) = 0
	float dot_AA = a.x * a.x + a.y * a.y;
	float dot_AB = a.x * b_coef.x + a.y * b_coef.y;
	float dot_AS = a.x * s.x + a.y * s.y;
	float dot_BB = b_coef.x * b_coef.x + b_coef.y * b_coef.y;
	float dot_BS = b_coef.x * s.x + b_coef.y * s.y;

	float a0 = 2.0f * dot_AA;
	float a1 = 3.0f * dot_AB;
	float a2 = 2.0f * dot_AS + dot_BB;
	float a3 = dot_BS;

	auto eval_cubic = [a0, a1, a2, a3](float t) { return ((a0 * t + a1) * t + a2) * t + a3; };
	auto eval_deriv = [a0, a1, a2](float t) { return (3.0f * a0 * t + 2.0f * a1) * t + a2; };

	float min_dist_sq = 1e9f;

	// check curve end points (t = 0 и t = 1)
	{
		float dx = p0.x - p.x, dy = p0.y - p.y;
		min_dist_sq = dx * dx + dy * dy;
	}
	{
		float dx = p2.x - p.x, dy = p2.y - p.y;
		min_dist_sq = std::min(min_dist_sq, dx * dx + dy * dy);
	}

	// Newton's method with 8 start points for absolute reliability
	for (int i = 0; i <= 8; ++i) {
		float t = static_cast<float>(i) / 8.0f;
		for (int iter = 0; iter < 4; ++iter) {
			float val = eval_cubic(t);
			float der = eval_deriv(t);
			if (std::abs(der) < 1e-6f) break;
			t -= val / der;
			if (t < 0.0f || t > 1.0f) break;
		}
		if (t >= 0.0f && t <= 1.0f) {
			float u = 1.0f - t;
			Point b_t = {
				u * u * p0.x + 2.0f * u * t * c.x + t * t * p2.x,
				u * u * p0.y + 2.0f * u * t * c.y + t * t * p2.y
			};
			float dx = b_t.x - p.x, dy = b_t.y - p.y;
			min_dist_sq = std::min(min_dist_sq, dx * dx + dy * dy);
		}
	}

	return std::sqrt(min_dist_sq);
}

// 3. Winding Number (if point is inside/outside)
inline bool is_inside(Point p, const GlyphOutlineContext& ctx) {
	int winding_number = 0;

	auto check_intersect = [&](Point a, Point b) {
		if (a.y <= p.y) {
			if (b.y > p.y && ((b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y)) > 0.0f)
				winding_number++;
		}
		else {
			if (b.y <= p.y && ((b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y)) < 0.0f)
				winding_number--;
		}
	};

	for (const auto& line : ctx.lines) {
		check_intersect(line.a, line.b);
	}
	for (const auto& curve : ctx.curves) {
		Point prev = curve.p0;
		const int steps = 8;
		for (int i = 1; i <= steps; ++i) {
			float t = (float)i / steps;
			float u = 1.0f - t;
			Point next = {
				u * u * curve.p0.x + 2.0f * u * t * curve.c.x + t * t * curve.p2.x,
				u * u * curve.p0.y + 2.0f * u * t * curve.c.y + t * t * curve.p2.y
			};
			check_intersect(prev, next);
			prev = next;
		}
	}
	return (winding_number != 0);
}

// FreeType Callbacks
inline int ft_move_to(const FT_Vector* to, void* user) {
	auto* ctx = static_cast<GlyphOutlineContext*>(user);
	ctx->current_point = { to->x / 64.0f, to->y / 64.0f };
	return 0;
}

inline int ft_line_to(const FT_Vector* to, void* user) {
	auto* ctx = static_cast<GlyphOutlineContext*>(user);
	Point p = { to->x / 64.0f, to->y / 64.0f };
	ctx->lines.push_back({ ctx->current_point, p });
	ctx->current_point = p;
	return 0;
}

inline int ft_conic_to(const FT_Vector* control, const FT_Vector* to, void* user) {
	auto* ctx = static_cast<GlyphOutlineContext*>(user);
	Point c = { control->x / 64.0f, control->y / 64.0f };
	Point end = { to->x / 64.0f, to->y / 64.0f };

	ctx->curves.push_back({ ctx->current_point, c, end });
	ctx->current_point = end;
	return 0;
}

inline int ft_cubic_to(const FT_Vector* ctrl1, const FT_Vector* ctrl2, const FT_Vector* to, void* user) {
	// .ttf fonts shouldn't use cubic, mostly they use quadratic, but this implementation should simplify cubic info quadratic
	auto* ctx = static_cast<GlyphOutlineContext*>(user);
	Point end = { to->x / 64.0f, to->y / 64.0f };
	ctx->lines.push_back({ ctx->current_point, end });
	ctx->current_point = end;
	return 0;
}

// 4. Main SDF generation function
inline void generate_vector_sdf_glyph(
	FT_Face face,
	uint32_t char_code,
	std::vector<uint8_t>& out_sdf_buffer,
	GlyphData* glyphData,
	int atlas_width = 1024,
	int atlas_height = 1024,
	int sdf_slot_size = 64,
	int spread = 8.0f
) {
	int font_pixel_size = sdf_slot_size - spread * 2;
	FT_Set_Pixel_Sizes(face, 0, 48);

	FT_UInt glyph_index = FT_Get_Char_Index(face, char_code);
	if (FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP)) return;

	FT_Outline outline = face->glyph->outline;
	GlyphOutlineContext ctx;
	FT_Outline_Funcs funcs = { ft_move_to, ft_line_to, ft_conic_to, ft_cubic_to, 0, 0 };
	FT_Outline_Decompose(&outline, &funcs, &ctx);

	// Glyph metrics for positioning
	float bearing_x = face->glyph->metrics.horiBearingX / 64.0f;
	float bearing_y = face->glyph->metrics.horiBearingY / 64.0f;
	int advance = face->glyph->advance.x / 64.0f;

	// set origin in 64x64 glyph box considering spread
	float origin_x = spread - bearing_x;
	float origin_y = sdf_slot_size * 0.75f; // baseline is on 3/4 of box height

	// slot coords in atlas (16x16 glyphs in 1024x1024 texture)
	int slot_x = ((char_code - 32) % 16) * sdf_slot_size;
	int slot_y = ((char_code - 32) / 16) * sdf_slot_size;

	// scan slot and get pixel_value (min distance)
	for (int y = 0; y < sdf_slot_size; ++y) {
		for (int x = 0; x < sdf_slot_size; ++x) {
			// get glyph local coords (inverted Y for FreeType)
			float px = static_cast<float>(x) - origin_x;
			float py = origin_y - static_cast<float>(y);
			Point p = { px, py };

			float min_dist = 1e9f;
			for (const auto& line : ctx.lines) {
				min_dist = std::min(min_dist, distance_to_line(p, line.a, line.b));
			}
			for (const auto& curve : ctx.curves) {
				min_dist = std::min(min_dist, distance_to_quadratic_bezier(p, curve.p0, curve.c, curve.p2));
			}

			bool inside = is_inside(p, ctx);
			float sd = inside ? min_dist : -min_dist;

			float v = 0.5f + (sd / spread);
			v = std::clamp(v, 0.0f, 1.0f) * 255.0f;

			int global_x = slot_x + x;
			int global_y = slot_y + y;
			uint8_t final_pixel_value = static_cast<uint8_t>(v);
			out_sdf_buffer[global_y * atlas_width + global_x] = final_pixel_value;
		}
	}

	//get glyph box bounds for UV and glyph_size_ratio based on sdf pixel values
	int glyph_min_x = sdf_slot_size, glyph_max_x = 0;
	int glyph_min_y = sdf_slot_size, glyph_max_y = 0;

	for (int x = 0; x < sdf_slot_size; ++x) {
		for (int y = 0; y < sdf_slot_size; ++y) {
			int atlas_x = slot_x + x;
			int atlas_y = slot_y + y;
			if (out_sdf_buffer[atlas_y * atlas_width + atlas_x] > 0) {
				if (x < glyph_min_x) glyph_min_x = x;
				if (x > glyph_max_x) glyph_max_x = x;
				if (y < glyph_min_y) glyph_min_y = y;
				if (y > glyph_max_y) glyph_max_y = y;
			}
		}
	}
	if (glyph_min_x > glyph_max_x) {
		glyph_min_x = 0; glyph_max_x = sdf_slot_size - 1;
		glyph_min_y = 0; glyph_max_y = sdf_slot_size - 1;
	}

	float font_scale = (float)font_pixel_size;

	float glyph_norm_width = float(glyph_max_x - glyph_min_x) / (float)font_scale; //normalized width
	float glyph_norm_height = float(glyph_max_y - glyph_min_y) / (float)font_scale; //normalized height

	float glyph_x_bearing = bearing_x / font_scale; //normalized y bearing
	float glyph_y_bearing = bearing_y / font_scale; //normalized x bearing
	float glyph_advance = (float)advance / font_scale; //normalized advance
	float U0, U1, V0, V1;
	U0 = (slot_x + glyph_min_x) / (float)atlas_width;
	U1 = (slot_x + glyph_max_x) / (float)atlas_width;
	V0 = (slot_y + glyph_min_y) / (float)atlas_height;
	V1 = (slot_y + glyph_max_y) / (float)atlas_height;

	GlyphData& data = glyphData[char_code - 32];
	//use inverted Vs, because atlas data is inverted on Y
	data.U0 = U0;
	data.U1 = U1;
	data.V0 = V1;
	data.V1 = V0;
	data.width_normalized = glyph_norm_width;
	data.height_normalized = glyph_norm_height;
	data.bearingY_normalized = glyph_y_bearing;
	data.bearingX_normalized = glyph_x_bearing;
	data.advance_normalized = glyph_advance;
}

static uint8_t get_minimal_distance_value_OLD(int start_x, int start_y, float spread, FT_Bitmap& bmp) {
	int width = bmp.width;
	int height = bmp.rows;

	// get first pixel value and decide if it's inside (alpha > 127) or outside
	uint8_t pixel_value = (start_x < 0 || start_x >= width ||
		start_y < 0 || start_y >= height) ? 0 : bmp.buffer[start_y * width + start_x];
	bool inside = pixel_value > 127;

	// init minimal square distance as max possible
	float min_dist_square = spread * spread;

	//set search bounds and correct them
	int min_x = start_x - spread; if (min_x < 0) min_x = 0;
	int max_x = start_x + spread; if (max_x >= width) max_x = width - 1;
	int min_y = start_y - spread; if (min_y < 0) min_y = 0;
	int max_y = start_y + spread; if (max_y >= height) max_y = height - 1;

	//check all pixels that are available
	for (int y = min_y; y <= max_y; ++y) {
		for (int x = min_x; x <= max_x; ++x) {
			uint8_t current_pixel_value = bmp.buffer[y * width + x];
			bool current_inside = current_pixel_value > 127;

			if (current_inside != inside) {
				float dX = static_cast<float>(x - start_x);
				float dY = static_cast<float>(y - start_y);
				float dist_square = dX * dX + dY * dY;
				min_dist_square = std::min(dist_square, min_dist_square);
			}
		}
	}

	float min_dist = std::sqrt(min_dist_square);
	float sd = inside ? -min_dist : min_dist;

	//normalize value in [0.0f, 255.0f]
	float v = 0.5f + (sd / spread);
	v = std::clamp(v, 0.0f, 1.0f) * 255.0f;

	return static_cast<uint8_t>(v);
}

static void get_glyph_SDF_data_OLD(
	std::vector<unsigned char>& out_sdf,
	GlyphData* glyphData,
	FT_Face face,
	uint32_t char_code,
	int sdf_size,
	int sdf_padding,
	int sdf_high_res_size,
	float spread
) {
	FT_UInt glyphIndex = FT_Get_Char_Index(face, char_code);

	float scale_factor = (float)sdf_high_res_size / sdf_size;
	int high_res_padding = scale_factor * sdf_padding;
	int pixel_size = sdf_high_res_size - high_res_padding * 2;

	FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
	FT_Bitmap& bmp = face->glyph->bitmap;

	int high_res_bearingX = face->glyph->metrics.horiBearingX >> 6;
	int high_res_bearingY = face->glyph->metrics.horiBearingY >> 6;
	int advance = face->glyph->advance.x >> 6;

	int sdf_atlas_x_offset = char_code % 16;
	int sdf_atlas_y_offset = char_code / 16 - 2;
	int sdf_atlas_width = sdf_size * 16;

	int atlas_x, atlas_y;
	int height_half_bias = (pixel_size - bmp.rows) / 2;
	int width_half_bias = (pixel_size - bmp.width) / 2;

	//setup sdf pixel values
	for (int y = 0; y < sdf_size; y++) {
		for (int x = 0; x < sdf_size; x++) {
			// map this coordinate on a high res bitmap
			// if high res size = 1024, usual size = 64 and padding = 8 --> high res bitmap desired size is 768, padding = 128, min is -128, max is 880)

			int high_res_x = x * scale_factor - high_res_padding - width_half_bias;
			int high_res_y = y * scale_factor - high_res_padding - height_half_bias;

			atlas_x = sdf_atlas_x_offset * sdf_size + x;
			atlas_y = sdf_atlas_y_offset * sdf_size + y;
			uint8_t pixel_value = get_minimal_distance_value_OLD(high_res_x, high_res_y, spread, bmp);
			out_sdf[atlas_y * sdf_atlas_width + atlas_x] = pixel_value;
		}
	}

	//get glyph box bounds for UV and glyph_size_ratio based on sdf pixel values
	int glyph_min_x = 0, glyph_max_x = sdf_size - 1;
	int glyph_min_y = 0, glyph_max_y = sdf_size - 1;

	for (int x = 0; x < sdf_size; x++) { //find min X bound of glyph box where pixel value > 0
		for (int y = 0; y < sdf_size; y++) {
			atlas_x = sdf_atlas_x_offset * sdf_size + x;
			atlas_y = sdf_atlas_y_offset * sdf_size + y;
			uint8_t pixel_value = out_sdf[atlas_y * sdf_atlas_width + atlas_x];
			if (pixel_value > 0) {
				glyph_min_x = x;
				x = sdf_size;
				break;
			}
		}
	}
	for (int y = 0; y < sdf_size; y++) { //find min Y bound of glyph box where pixel value > 0
		for (int x = 0; x < sdf_size; x++) {
			atlas_x = sdf_atlas_x_offset * sdf_size + x;
			atlas_y = sdf_atlas_y_offset * sdf_size + y;
			uint8_t pixel_value = out_sdf[atlas_y * sdf_atlas_width + atlas_x];
			if (pixel_value > 0) {
				glyph_min_y = y;
				y = sdf_size;
				break;
			}
		}
	}
	for (int x = sdf_size - 1; x >= 0; x--) { //find max X bound of glyph box where pixel value > 0
		for (int y = sdf_size - 1; y >= 0; y--) {
			atlas_x = sdf_atlas_x_offset * sdf_size + x;
			atlas_y = sdf_atlas_y_offset * sdf_size + y;
			uint8_t pixel_value = out_sdf[atlas_y * sdf_atlas_width + atlas_x];
			if (pixel_value > 0) {
				glyph_max_x = x;
				x = -1;
				break;
			}
		}
	}
	for (int y = sdf_size - 1; y >= 0; y--) { //find max Y bound of glyph box where pixel value > 0
		for (int x = sdf_size - 1; x >= 0; x--) {
			atlas_x = sdf_atlas_x_offset * sdf_size + x;
			atlas_y = sdf_atlas_y_offset * sdf_size + y;
			uint8_t pixel_value = out_sdf[atlas_y * sdf_atlas_width + atlas_x];
			if (pixel_value > 0) {
				glyph_max_y = y;
				y = -1;
				break;
			}
		}
	}

	float glyph_norm_width = float(glyph_max_x - glyph_min_x) / (sdf_size - sdf_padding * 2); //normalized width
	float glyph_norm_height = float(glyph_max_y - glyph_min_y) / (sdf_size - sdf_padding * 2); //normalized height

	float glyph_y_bearing = (float)high_res_bearingY / pixel_size; //normalized y bearing
	float glyph_x_bearing = (float)high_res_bearingX / pixel_size; //normalized x bearing
	float glyph_advance = (float)advance / pixel_size; //normalized advance
	float U0, U1, V0, V1; //UVs
	U0 = (sdf_atlas_x_offset * sdf_size + glyph_min_x) / (float)sdf_atlas_width;
	V0 = (sdf_atlas_y_offset * sdf_size + glyph_min_y) / (float)sdf_atlas_width;
	U1 = (sdf_atlas_x_offset * sdf_size + glyph_max_x) / (float)sdf_atlas_width;
	V1 = (sdf_atlas_y_offset * sdf_size + glyph_max_y) / (float)sdf_atlas_width;

	GlyphData& data = glyphData[char_code - 32];
	//use inverted UVs, because atlas data is inverted on Y
	data.U0 = U0;
	data.U1 = U1;
	data.V0 = V1;
	data.V1 = V0;
	data.width_normalized = glyph_norm_width;
	data.height_normalized = glyph_norm_height;
	data.bearingY_normalized = glyph_y_bearing;
	data.bearingX_normalized = glyph_x_bearing;
	data.advance_normalized = glyph_advance;
}

bool FontGenerator::generate_truetype_sdf_font(std::filesystem::path& fontPath, bool write_atlas_to_png) {
	// FreeType setup
	FT_Library ft = nullptr;
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "[FontGenerator] Error: Couldn't load FreeType library!\n";
		return false;
	}

	FT_Face face = nullptr;
	if (FT_New_Face(ft, fontPath.string().c_str(), 0, &face)) {
		std::cerr << "[FontGenerator] Error: Couldn't load font file: " << fontPath << std::endl;
		return false;
	}

	int atlas_size = 1024;
	int glyph_slot_size = 64;
	int spread = 8;

	int font_scale = glyph_slot_size - spread * 2;
	FT_Set_Pixel_Sizes(face, 0, font_scale);

	float normalized_ascender = (float(face->size->metrics.ascender) / 64.0f) / (float)font_scale;
	float normalized_descender = (float(face->size->metrics.descender) / 64.0f) / (float)font_scale;

	//font bitmap creation
	uint32_t first = 33, last = 127;
	std::vector<uint32_t> charset;
	charset.reserve(size_t(last - first));
	for (char32_t cp = first; cp < last; ++cp) {
		charset.push_back(cp);
	}

	uint32_t glyph_atlas_size = 1024;
	//sdf font atlas bitmap
	std::vector<unsigned char> sdf(glyph_atlas_size * glyph_atlas_size, 0);

	//glyphs data
	GlyphData glyphsData[95];

	//setup bitmap and glyphs data
	for (auto& code : charset) {
		//std::cout << "	Code = " << code << ", Glyph: " << char(code) << std::endl;
		generate_vector_sdf_glyph(face, code, sdf, glyphsData, atlas_size, atlas_size, glyph_slot_size, spread);
	}

	std::filesystem::path files_path;
	std::filesystem::path file_path;
	std::string new_file_name;
	std::string filename = fontPath.filename().string();
	std::filesystem::path sdf_name = filename.substr(0, filename.find_last_of('.')) + "_SDF";
	files_path = output_fonts_folder / sdf_name;
	std::filesystem::create_directory(files_path);

	//save atlas bytes to file
	new_file_name = sdf_name.string() + ".atlas";
	file_path = files_path / new_file_name;
	std::ofstream atlas_file(file_path, std::ios::binary);

	atlas_file.write(reinterpret_cast<const char*>(&glyph_atlas_size), sizeof(glyph_atlas_size));
	atlas_file.write(reinterpret_cast<const char*>(sdf.data()), sdf.size());
	atlas_file.close();

	//save atlas meta to file
	new_file_name = sdf_name.string() + ".meta";
	file_path = files_path / new_file_name;
	std::ofstream meta_file(file_path, std::ios::binary);

	uint32_t glyphs_count = 95;
	meta_file.write(reinterpret_cast<const char*>(&glyphs_count), sizeof(glyphs_count));
	meta_file.write(reinterpret_cast<const char*>(&normalized_ascender), sizeof(normalized_ascender));
	meta_file.write(reinterpret_cast<const char*>(&normalized_descender), sizeof(normalized_descender));
	for (auto& glyph : glyphsData) {
		meta_file.write(reinterpret_cast<const char*>(&glyph), sizeof(GlyphData));
	}
	meta_file.close();

	//write atlas img to png if needed
	if (write_atlas_to_png) {
		new_file_name = sdf_name.string() + ".png";
		file_path = files_path / new_file_name;
		stbi_write_png(file_path.string().c_str(), glyph_atlas_size, glyph_atlas_size, 1, sdf.data(), glyph_atlas_size);
	}

	return true;
}