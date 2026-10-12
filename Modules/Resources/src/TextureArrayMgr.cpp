#include "Resources/TextureArrayMgr.h"
#include <stb_image.h>

void CoreResource::TextureArrayManager::init() {
	const uint32_t array_size[8] = {
		10/*32*/,
		10/*64*/,
		10/*128*/,
		10/*256*/,
		10/*512*/,
		0/*1024*/,
		0/*2048*/,
		0/*4096*/
	};
	const TextureArray::ResizeStrategy array_resize_str[8] = {
		TextureArray::ResizeStrategy::Resize_by_50_percent,
		TextureArray::ResizeStrategy::Resize_by_50_percent,
		TextureArray::ResizeStrategy::Resize_by_50_percent,
		TextureArray::ResizeStrategy::Resize_by_50_percent,
		TextureArray::ResizeStrategy::Resize_by_50_percent,
		TextureArray::ResizeStrategy::Resize_by_5,
		TextureArray::ResizeStrategy::Resize_by_2,
		TextureArray::ResizeStrategy::Resize_by_1,
	};
	uint32_t tex_res = 32;
	for (uint32_t i = 0; i < 8; ++i) {
		texture_arrays[i] = std::make_unique<TextureArray>();
		texture_arrays[i]->setup_texture_array(tex_res, tex_res, array_size[i], false, array_resize_str[i]);
		tex_res *= 2;
	}
}

void CoreResource::TextureArrayManager::uninit() {
	for (uint32_t i = 0; i < 8; ++i) {
		texture_arrays[i].reset();
	}
}

void CoreResource::TextureArrayManager::load_texture(std::filesystem::path file_path, std::string UID) {
	int imgWidth, imgHeight, numOfColorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_bytes = stbi_load(file_path.string().c_str(), &imgWidth, &imgHeight, &numOfColorChannels, 0);

	int texture_array_index = get_array_index(imgWidth, imgHeight);
	if (texture_array_index < 0) {
		stbi_set_flip_vertically_on_load(false);
		stbi_image_free(image_bytes);
		throw std::runtime_error("Texture <" + file_path.string() + "> has incorrect size!");
	}

	uint16_t tex_index = texture_arrays[texture_array_index]->get_current_size();
	UID_to_ID[UID] = TextureID{ static_cast<uint16_t>(texture_array_index), tex_index };

	texture_arrays[texture_array_index]->add_2D_texture(image_bytes, imgWidth, imgHeight, numOfColorChannels);

	stbi_set_flip_vertically_on_load(false);
	stbi_image_free(image_bytes);
}

std::optional<CoreResource::TextureID> CoreResource::TextureArrayManager::get_texture_id(std::string& UID) {
	if (auto it = UID_to_ID.find(UID); it != UID_to_ID.end()) {
		return it->second;
	}
	else return std::nullopt;
}

int CoreResource::TextureArrayManager::get_array_index(uint32_t tex_width, uint32_t tex_height) {
	if (tex_width != tex_height) return -1;
	switch (tex_width) {
		case 32:   return 0;
		case 64:   return 1;
		case 128:  return 2;
		case 256:  return 3;
		case 512:  return 4;
		case 1024: return 5;
		case 2048: return 6;
		case 4096: return 7;
		default:   return -1;
	}
}
