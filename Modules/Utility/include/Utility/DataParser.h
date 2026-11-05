#pragma once
#include <filesystem>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace Engine::Util {
	class DataParser {
		enum DataNodeType : uint8_t {
			Node_NULL, Node_Value, Node_Object, Node_Array
		};
	public:
		enum JsonValue : uint8_t { V_INT, V_UINT, V_FLOAT, V_BOOL, V_STRING, V_OBJECT };
	public:
		bool parse_data(std::filesystem::path& path, bool parse_json_file);
		bool parse_binary_format(std::filesystem::path& path);
		bool parse_JSON_format(std::filesystem::path& path);

		bool parse_JSON_to_binary(std::filesystem::path& path);
		void write_JSON_to_binary(std::vector<uint8_t>& out_data, const nlohmann::json& j);

		void write_from_binary(std::vector<uint8_t>& in, nlohmann::json& j, uint64_t& pointer_offset);

		static bool read_text_file(std::filesystem::path& path, std::string& str);
		template<typename T, DataParser::JsonValue value>
		static bool get_json_value(T& field, const nlohmann::json& j, const std::string& field_name);
		template<typename T, DataParser::JsonValue value>
		static bool get_json_array(T& field, const nlohmann::json& j, const std::string& field_name);

		nlohmann::json node_root;
	private:
		void append_data_to_bin_buffer(std::vector<uint8_t>& bin_data, const void* data_ptr, size_t data_size);
		void read_data_from_bin_buffer(const std::vector<uint8_t>& bin_data, uint64_t& pointer_offset, void* data_ptr, size_t data_size);
	};

	template<typename T, DataParser::JsonValue value>
	bool DataParser::get_json_value(T& field, const nlohmann::json& j, const std::string& field_name) {
		if (auto it = j.find(field_name); it != j.end()) {
			switch (value) {
				case DataParser::JsonValue::V_INT:
					if (!it->is_number_integer()) return false;
					break;
				case DataParser::JsonValue::V_UINT:
					if (!it->is_number_unsigned()) return false;
					break;
				case DataParser::JsonValue::V_FLOAT:
					if (!it->is_number_float()) return false;
					break;
				case DataParser::JsonValue::V_BOOL:
					if (!it->is_boolean()) return false;
					break;
				case DataParser::JsonValue::V_STRING:
					if (!it->is_string()) return false;
					break;
				case DataParser::JsonValue::V_OBJECT:
					if (!it->is_object()) return false;
					break;
			}
			field = it->get<T>();
			return true;
		}
		return false;
	}

	template<typename T, DataParser::JsonValue value>
	bool DataParser::get_json_array(T& field, const nlohmann::json& j, const std::string& field_name) {
		if (auto it = j.find(field_name); it != j.end()) {
			if (!it->is_array()) return false;

			switch (value) {
			case DataParser::JsonValue::V_INT:
				for (const auto& element : *it)
					if (!element.is_number_integer()) return false;
				break;
			case DataParser::JsonValue::V_UINT:
				for (const auto& element : *it)
					if (!element.is_number_unsigned()) return false;
				break;
			case DataParser::JsonValue::V_FLOAT:
				for (const auto& element : *it)
					if (!element.is_number_float()) return false;
				break;
			case DataParser::JsonValue::V_BOOL:
				for (const auto& element : *it)
					if (!element.is_boolean()) return false;
				break;
			case DataParser::JsonValue::V_STRING:
				for (const auto& element : *it)
					if (!element.is_string()) return false;
				break;
			case DataParser::JsonValue::V_OBJECT:
				for (const auto& element : *it)
					if (!element.is_object()) return false;
				break;
			}
			field = it->get<T>();
			return true;
		}
		return false;
	}
}
