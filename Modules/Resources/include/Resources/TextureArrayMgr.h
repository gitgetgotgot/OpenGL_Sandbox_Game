#pragma once
#include <Rendering/TextureArray.h>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <queue>

namespace CoreResource {
	struct TextureID {
		uint16_t array_id = 0;
		uint16_t index = 0;
	};

	struct TextureResourceData {
		TextureResourceData() = default;
		TextureResourceData(
			unsigned char* image_bytes,
			int imgWidth, int imgHeight, int numOfColorChannels,
			std::filesystem::path& file_path, std::string& UID
		) : image_bytes{ image_bytes }, imgWidth{ imgWidth },
			imgHeight{ imgHeight }, file_path{ file_path }, UID{ UID } {}
		TextureResourceData(const TextureResourceData&) = delete;
		TextureResourceData& operator=(const TextureResourceData&) = delete;
		TextureResourceData(TextureResourceData&&);
		TextureResourceData& operator=(TextureResourceData&&);
		~TextureResourceData();
		unsigned char* image_bytes;
		int imgWidth, imgHeight, numOfColorChannels;
		std::filesystem::path file_path;
		std::string UID;
	};

	class TextureArrayManager {
	public:
		static TextureArrayManager& Instance() {
			static TextureArrayManager mgr;
			return mgr;
		}
		void init();
		void uninit();
		void add_texture_to_queue(std::filesystem::path& file_path, std::string& UID);
		bool load_textures_from_queue();
		void mark_queue_ready();
		void load_texture(std::filesystem::path file_path, std::string UID);
		void load_texture(TextureResourceData& data);
		std::optional<TextureID> get_texture_id(std::string& UID);
		uint32_t get_texture_array_res(uint16_t tex_array_id);
	private:
		int get_array_index(uint32_t tex_width, uint32_t tex_height);
		TextureArrayManager() = default;
		~TextureArrayManager() = default;

		// 32x32, 64x64, 128x128, 256x256, 512x512, 1024x1024, 2048x2048, 4096x4096
		std::unique_ptr<TextureArray> texture_arrays[8];
		std::unordered_map<std::string, TextureID> UID_to_ID;

		std::queue<TextureResourceData> textures_queue;
		std::mutex tex_queue_mutex;
		std::atomic_bool queue_ready;
	};
}