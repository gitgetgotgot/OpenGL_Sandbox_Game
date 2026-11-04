#include <Utility/DataParser.h>
#include <Utility/Logger.h>
#include <Utility/Hash.h>
#include <fstream>

bool Engine::Util::DataParser::parse_data(std::filesystem::path& path, bool parse_json_file) {
	if (parse_json_file) return parse_JSON_format(path);
	else return parse_binary_format(path);
}

bool Engine::Util::DataParser::parse_binary_format(std::filesystem::path& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		Engine::Util::Logger::Log("Data Parser: Cannot open file: " + path.string());
		return false;
	}

	std::vector<uint8_t> bin_data;

	uint64_t file_hash = 0;
	file.read(reinterpret_cast<char*>(&file_hash), sizeof(file_hash));

	uint64_t data_size;
	file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));

	bin_data.resize(data_size);
	file.read(reinterpret_cast<char*>(bin_data.data()), data_size);

	uint64_t file_real_hash = Engine::Util::Hash::FNV_1a_64(bin_data);

	if (file_hash != file_real_hash) {
		Engine::Util::Logger::Log("Data Parser: Corrupted file: " + path.string());
		return false;
	}

	uint64_t pointer_offset = 0;
	write_from_binary(bin_data, node_root, pointer_offset);

	return true;
}

bool Engine::Util::DataParser::parse_JSON_format(std::filesystem::path& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		Engine::Util::Logger::Log("Data Parser: Cannot open file: " + path.string());
		return false;
	}
	node_root = nlohmann::json::parse(file, nullptr, false);
	if (node_root.is_discarded()) {
		Engine::Util::Logger::Log("Data Parser: Invalid syntax in JSON file: " + path.string());
		return false;
	}
	return true;
}

bool Engine::Util::DataParser::parse_JSON_to_binary(std::filesystem::path& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		Engine::Util::Logger::Log("Data Parser: Cannot open file: " + path.string());
		return false;
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	if (json.is_discarded()) {
		Engine::Util::Logger::Log("Data Parser: Invalid syntax in JSON file: " + path.string());
		return false;
	}

	std::filesystem::path new_file_path = path;
	new_file_path.replace_extension(".data");
	std::ofstream bin_file(new_file_path, std::ios::binary);
	if (!bin_file.is_open()) {
		Engine::Util::Logger::Log("Data Parser: Cannot create binary file: " + new_file_path.string());
		return false;
	}

	std::vector<uint8_t> bin_data;
	write_JSON_to_binary(bin_data, json);

	uint64_t file_hash = 0;
	bin_file.write(reinterpret_cast<const char*>(&file_hash), sizeof(file_hash));

	uint64_t data_size = bin_data.size();
	bin_file.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));

	bin_file.write(reinterpret_cast<const char*>(bin_data.data()), data_size);

	file_hash = Engine::Util::Hash::FNV_1a_64(bin_data);

	bin_file.seekp(0);
	bin_file.write(reinterpret_cast<const char*>(&file_hash), sizeof(file_hash));

	return true;
}

void Engine::Util::DataParser::write_JSON_to_binary(std::vector<uint8_t>& out_data, const nlohmann::json& j) {
	if (j.is_object()) {
		DataNodeType type = DataNodeType::Node_Object;
		append_data_to_bin_buffer(out_data, &type, sizeof(type));

		uint32_t size = j.size();
		append_data_to_bin_buffer(out_data, &size, sizeof(size));

		for (auto& pair : j.items()) {
			std::string child_name = pair.key();
			for (auto& c : child_name) c ^= 0x45;
			uint32_t len = child_name.size();
			append_data_to_bin_buffer(out_data, &len, sizeof(len));
			append_data_to_bin_buffer(out_data, child_name.data(), len);

			write_JSON_to_binary(out_data, pair.value());
		}
	}
	else if (j.is_array()) {
		DataNodeType type = DataNodeType::Node_Array;
		append_data_to_bin_buffer(out_data, &type, sizeof(type));

		uint32_t array_size = j.size();
		append_data_to_bin_buffer(out_data, &array_size, sizeof(array_size));

		for (auto& val : j) write_JSON_to_binary(out_data, val);
	}
	else if (j.is_number()) {
		DataNodeType type = DataNodeType::Node_Value;
		append_data_to_bin_buffer(out_data, &type, sizeof(type));

		std::string value = j.dump();
		for (auto& c : value) c ^= 0x45;
		uint32_t len = value.size();
		append_data_to_bin_buffer(out_data, &len, sizeof(len));
		append_data_to_bin_buffer(out_data, value.data(), len);
	}
	else if (j.is_string()) {
		DataNodeType type = DataNodeType::Node_Value;
		append_data_to_bin_buffer(out_data, &type, sizeof(type));

		std::string value = j.get<std::string>();
		for (auto& c : value) c ^= 0x45;
		uint32_t len = value.size();
		append_data_to_bin_buffer(out_data, &len, sizeof(len));
		append_data_to_bin_buffer(out_data, value.data(), len);
	}
	else if (j.is_boolean()) {
		DataNodeType type = DataNodeType::Node_Value;
		append_data_to_bin_buffer(out_data, &type, sizeof(type));

		std::string value = j.get<bool>() ? "true" : "false";
		for (auto& c : value) c ^= 0x45;
		uint32_t len = value.size();
		append_data_to_bin_buffer(out_data, &len, sizeof(len));
		append_data_to_bin_buffer(out_data, value.data(), len);
	}
	else if (j.is_null()) {
		DataNodeType type = DataNodeType::Node_NULL;
		append_data_to_bin_buffer(out_data, &type, sizeof(type));
	}
}

void Engine::Util::DataParser::write_from_binary(std::vector<uint8_t>& in, nlohmann::json& j, uint64_t& pointer_offset) {
	DataNodeType type{};
	read_data_from_bin_buffer(in, pointer_offset, &type, sizeof(type));

	if (type == DataNodeType::Node_Object) {
		j = nlohmann::json::object();

		uint32_t size{};
		read_data_from_bin_buffer(in, pointer_offset, &size, sizeof(size));
		for (uint32_t i = 0; i < size; i++) {
			uint32_t len{};
			read_data_from_bin_buffer(in, pointer_offset, &len, sizeof(len));
			std::string child_name; child_name.resize(len);
			read_data_from_bin_buffer(in, pointer_offset, child_name.data(), len);
			for (auto& c : child_name) c ^= 0x45;

			write_from_binary(in, j[child_name], pointer_offset);
		}
	}
	else if (type == DataNodeType::Node_Array) {
		j = nlohmann::json::array();

		uint32_t array_size{};
		read_data_from_bin_buffer(in, pointer_offset, &array_size, sizeof(array_size));
		j.get_ref<nlohmann::json::array_t&>().reserve(array_size);

		for (uint32_t i = 0; i < array_size; i++) {
			write_from_binary(in, j.emplace_back(), pointer_offset);
		}
	}
	else if (type == DataNodeType::Node_Value) {
		uint32_t len{};
		read_data_from_bin_buffer(in, pointer_offset, &len, sizeof(len));
		std::string value_str;
		value_str.resize(len);
		read_data_from_bin_buffer(in, pointer_offset, value_str.data(), len);
		for (auto& c : value_str) c ^= 0x45;

		if (value_str == "true") j = true;
		else if (value_str == "false") j = false;
		else {
			// number or string
			j = nlohmann::json::parse(value_str, nullptr, false);
			if (j.is_discarded()) {
				j = value_str; // if can't parse as number, then it's string
			}
		}
	}
	else {
		j = nullptr;
	}
}

bool Engine::Util::DataParser::read_text_file(std::filesystem::path& path, std::string& str) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		Engine::Util::Logger::Log("Data Parser: Cannot open file: " + path.string());
		return false;
	}
	file.seekg(0, std::ios::end);
	size_t count = file.tellg();
	file.seekg(0, std::ios::beg);
	str.resize(count);
	file.read(str.data(), count);

	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

	return true;
}

void Engine::Util::DataParser::append_data_to_bin_buffer(std::vector<uint8_t>& bin_data, const void* data_ptr, size_t data_size) {
	size_t old_size = bin_data.size();
	bin_data.resize(old_size + data_size);
	std::memcpy(bin_data.data() + old_size, data_ptr, data_size);
}

void Engine::Util::DataParser::read_data_from_bin_buffer(const std::vector<uint8_t>& bin_data, uint64_t& pointer_offset, void* data_ptr, size_t data_size) {
	std::memcpy(data_ptr, bin_data.data() + pointer_offset, data_size);
	pointer_offset += data_size;
}