#include "Resources/TextureArrayMgr.h"
#include <stb_image.h>
#include <glad/gl.h>

Engine::Resource::TextureArrayManager::TextureResourceData::TextureResourceData(TextureResourceData&& other) {
	image_bytes = other.image_bytes; other.image_bytes = nullptr;
	imgWidth = other.imgWidth;
	imgHeight = other.imgHeight;
	numOfColorChannels = other.numOfColorChannels;
	file_path = std::move(other.file_path);
	UID = std::move(other.UID);
}

Engine::Resource::TextureArrayManager::TextureResourceData&
Engine::Resource::TextureArrayManager::TextureResourceData::operator=(TextureResourceData&& other) {
	if (this != &other) {
		image_bytes = other.image_bytes; other.image_bytes = nullptr;
		imgWidth = other.imgWidth;
		imgHeight = other.imgHeight;
		numOfColorChannels = other.numOfColorChannels;
		file_path = std::move(other.file_path);
		UID = std::move(other.UID);
	}
	return *this;
}

Engine::Resource::TextureArrayManager::TextureResourceData::~TextureResourceData() {
	if(image_bytes) stbi_image_free(image_bytes);
}

void Engine::Resource::TextureArrayManager::init() {
	// read texture bytes without alignment or padding (correctly load textures of different formats and sizes)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
	const Engine::Graphics::TextureArray::ResizeStrategy array_resize_str[8] = {
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_50_percent,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_50_percent,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_50_percent,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_50_percent,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_50_percent,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_5,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_2,
		Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_1,
	};
	uint32_t tex_res = 32;
	for (uint32_t i = 0; i < 8; ++i) {
		texture_arrays[i] = std::make_unique<Engine::Graphics::TextureArray>();
		texture_arrays[i]->setup_texture_array(tex_res, tex_res, array_size[i], false, 4, array_resize_str[i]);
		tex_res *= 2;
	}
	fonts_texture_array = std::make_unique<Engine::Graphics::TextureArray>();
	fonts_texture_array->setup_texture_array(1024, 1024, 1, true, 1, Engine::Graphics::TextureArray::ResizeStrategy::Resize_by_1);

	for (uint32_t i = 0; i < 8; ++i) {
		texture_arrays[i]->bind(i);
	}
	fonts_texture_array->bind(10);
}

void Engine::Resource::TextureArrayManager::uninit() {
	for (uint32_t i = 0; i < 8; ++i) {
		texture_arrays[i].reset();
	}
	fonts_texture_array.reset();
}

void Engine::Resource::TextureArrayManager::bind_texture_arrays() {
	for (uint32_t i = 0; i < 8; ++i) {
		texture_arrays[i]->bind(i);
	}
	fonts_texture_array->bind(10);
}

void Engine::Resource::TextureArrayManager::add_texture_to_queue(std::filesystem::path& file_path, std::string& UID) {
	int imgWidth, imgHeight, numOfColorChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_bytes = stbi_load(file_path.string().c_str(), &imgWidth, &imgHeight, &numOfColorChannels, 0);
	stbi_set_flip_vertically_on_load(false);

	std::lock_guard lock(tex_queue_mutex);
	textures_queue.emplace(image_bytes, imgHeight, imgWidth, numOfColorChannels, file_path, UID);
}

bool Engine::Resource::TextureArrayManager::load_textures_from_queue() {
	std::unique_lock lock(tex_queue_mutex);
	if (textures_queue.empty()) {
		if (queue_ready) return true;
		else return false;
	}
	TextureResourceData data = std::move(textures_queue.front());
	textures_queue.pop();
	lock.unlock();
	load_texture(data);
	return false;
}

void Engine::Resource::TextureArrayManager::mark_queue_ready() {
	queue_ready = true;
}

void Engine::Resource::TextureArrayManager::load_texture(std::filesystem::path file_path, std::string UID) {
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

void Engine::Resource::TextureArrayManager::load_texture(TextureResourceData& data) {
	int texture_array_index = get_array_index(data.imgWidth, data.imgHeight);
	if (texture_array_index < 0)
		throw std::runtime_error("Texture <" + data.file_path.string() + "> has incorrect size!");

	uint16_t tex_index = texture_arrays[texture_array_index]->get_current_size();
	UID_to_ID[data.UID] = TextureID{ static_cast<uint16_t>(texture_array_index), tex_index };

	texture_arrays[texture_array_index]->add_2D_texture(data.image_bytes, data.imgWidth, data.imgHeight, data.numOfColorChannels);
}

void Engine::Resource::TextureArrayManager::load_font_texture(unsigned char* data, int width, int height, int channels, std::string UID) {
	font_UID_to_ID[UID] = fonts_texture_array->get_current_size();
	fonts_texture_array->add_2D_texture(data, width, height, channels);
}

std::optional<Engine::Resource::TextureArrayManager::TextureID>
Engine::Resource::TextureArrayManager::get_texture_id(std::string& UID) {
	if (auto it = UID_to_ID.find(UID); it != UID_to_ID.end()) {
		return it->second;
	}
	else return std::nullopt;
}

std::optional<uint16_t> Engine::Resource::TextureArrayManager::get_font_texture_id(std::string& UID) {
	if (auto it = font_UID_to_ID.find(UID); it != font_UID_to_ID.end()) {
		return it->second;
	}
	else return std::nullopt;
}

uint32_t Engine::Resource::TextureArrayManager::get_texture_array_res(uint16_t tex_array_id) {
	switch (tex_array_id) {
	case 0: return 32;
	case 1: return 64;
	case 2: return 128;
	case 3: return 256;
	case 4: return 512;
	case 5: return 1024;
	case 6: return 2048;
	case 7: return 4096;
	default: return 0;
	}
}

int Engine::Resource::TextureArrayManager::get_array_index(uint32_t tex_width, uint32_t tex_height) {
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
