#pragma once
#include <unordered_map>
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Engine::Util {
	class DataParser {
		enum DataNodeType : uint8_t {
			Node_NULL, Node_Value, Node_Object, Node_Array
		};
	public:
		bool parse_data(std::filesystem::path& path, bool parse_json_file);
		bool parse_binary_format(std::filesystem::path& path);
		bool parse_JSON_format(std::filesystem::path& path);

		bool parse_JSON_to_binary(std::filesystem::path& path);
		void write_JSON_to_binary(std::vector<uint8_t>& out_data, const nlohmann::json& j);

		void write_from_binary(std::vector<uint8_t>& in, nlohmann::json& j, uint64_t& pointer_offset);

		bool read_text_file(std::filesystem::path& path, std::string& str);

		nlohmann::json node_root;
	private:
		void append_data_to_bin_buffer(std::vector<uint8_t>& bin_data, const void* data_ptr, size_t data_size);
		void read_data_from_bin_buffer(const std::vector<uint8_t>& bin_data, uint64_t& pointer_offset, void* data_ptr, size_t data_size);
	};
}