#include <Utility/DataParser.h>

const DataNode& DataNode::operator[](std::string key) const {
	auto it = children.find(key);
	if (it == children.end())
		throw std::runtime_error("Child <" + key + "> not found");
	return it->second;
}

bool DataNode::has_child(std::string name) const {
	return children.find(name) != children.end();
}

void DataParser::parse_data(std::filesystem::path& path, bool DEBUG) {
	if (DEBUG) return parse_JSON_format(path);
	else return parse_binary_format(path);
}

void DataParser::parse_binary_format(std::filesystem::path& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("[Data Parser] Cannot open file: " + path.string());
	try{
		write_from_binary(file, node_root);
	}
	catch (const std::exception& e) {
		throw std::runtime_error("File " + path.string() + " is corrupted: " + e.what());
	}
}

void DataParser::parse_JSON_format(std::filesystem::path& path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("[Data Parser] Cannot open file: " + path.string());
	nlohmann::json json;
	file >> json;
	write_from_JSON(json, node_root);
}

void DataParser::parse_JSON_to_binary(std::filesystem::path& path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("[Data Parser] Cannot open file: " + path.string());
	nlohmann::json json;
	file >> json;

	std::filesystem::path new_file_path = path;
	new_file_path.replace_extension(".DATA");
	std::ofstream bin_file(new_file_path, std::ios::binary);

	write_JSON_to_binary(bin_file, json);
}

void DataParser::write_from_JSON(const nlohmann::json& j, DataNode& data) {
	if (j.is_object()) {
		data.node_type = DataNodeType::Node_Object;
		for (auto& pair : j.items()) {
			DataNode& child = data.children[pair.key()];
			write_from_JSON(pair.value(), child);
		}
	}
	else if (j.is_array()) {
		data.node_type = DataNodeType::Node_Array;
		for (auto& val : j) {
			DataNode& child = data.array.emplace_back();
			write_from_JSON(val, child);
		}
	}
	else if (j.is_number()) {
		data.node_type = DataNodeType::Node_Value;
		data.value = j.dump();
	}
	else if (j.is_string()) {
		data.node_type = DataNodeType::Node_Value;
		data.value = j.get<std::string>();
	}
	else if (j.is_boolean()) {
		data.node_type = DataNodeType::Node_Value;
		data.value = j.get<bool>() ? "true" : "false";
	}
	else if (j.is_null()) {
		data.node_type = DataNodeType::Node_NULL;
	}
}

void DataParser::write_JSON_to_binary(std::ofstream& out, const nlohmann::json& j) {
	if (j.is_object()) {
		DataNodeType type = DataNodeType::Node_Object;
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));

		uint32_t size = j.size();
		out.write(reinterpret_cast<const char*>(&size), sizeof(size));

		for (auto& pair : j.items()) {
			std::string child_name = pair.key();
			for (auto& c : child_name) c ^= 0x45;
			uint32_t len = child_name.size();
			out.write(reinterpret_cast<const char*>(&len), sizeof(len));
			out.write(child_name.data(), len);

			write_JSON_to_binary(out, pair.value());
		}
	}
	else if (j.is_array()) {
		DataNodeType type = DataNodeType::Node_Array;
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));

		uint32_t array_size = j.size();
		out.write(reinterpret_cast<const char*>(&array_size), sizeof(array_size));

		for (auto& val : j) write_JSON_to_binary(out, val);
	}
	else if (j.is_number()) {
		DataNodeType type = DataNodeType::Node_Value;
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));

		std::string value = j.dump();
		for (auto& c : value) c ^= 0x45;
		uint32_t len = value.size();
		out.write(reinterpret_cast<const char*>(&len), sizeof(len));
		out.write(value.data(), len);
	}
	else if (j.is_string()) {
		DataNodeType type = DataNodeType::Node_Value;
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));

		std::string value = j.get<std::string>();
		for (auto& c : value) c ^= 0x45;
		uint32_t len = value.size();
		out.write(reinterpret_cast<const char*>(&len), sizeof(len));
		out.write(value.data(), len);
	}
	else if (j.is_boolean()) {
		DataNodeType type = DataNodeType::Node_Value;
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));

		std::string value = j.get<bool>() ? "true" : "false";
		for (auto& c : value) c ^= 0x45;
		uint32_t len = value.size();
		out.write(reinterpret_cast<const char*>(&len), sizeof(len));
		out.write(value.data(), len);
	}
	else if (j.is_null()) {
		DataNodeType type = DataNodeType::Node_NULL;
		out.write(reinterpret_cast<const char*>(&type), sizeof(type));
	}
}

void DataParser::write_from_binary(std::ifstream& in, DataNode& data) {
	DataNodeType type{};
	in.read(reinterpret_cast<char*>(&type), sizeof(type));
	data.node_type = type;

	if (type == DataNodeType::Node_Object) {
		uint32_t size{};
		in.read(reinterpret_cast<char*>(&size), sizeof(size));
		data.children.reserve(size);
		for (uint32_t i = 0; i < size; i++) {
			uint32_t len{};
			in.read(reinterpret_cast<char*>(&len), sizeof(len));
			std::string child_name; child_name.resize(len);
			in.read(child_name.data(), len);
			for (auto& c : child_name) c ^= 0x45;

			DataNode& child = data.children[child_name];
			write_from_binary(in, child);
		}
	}
	else if (type == DataNodeType::Node_Array) {
		uint32_t array_size{};
		in.read(reinterpret_cast<char*>(&array_size), sizeof(array_size));
		data.array.reserve(array_size);
		for (uint32_t i = 0; i < array_size; i++) {
			DataNode& node = data.array.emplace_back();
			write_from_binary(in, node);
		}
	}
	else if (type == DataNodeType::Node_Value) {
		uint32_t len{};
		in.read(reinterpret_cast<char*>(&len), sizeof(len));
		std::string value; value.resize(len);
		in.read(value.data(), len);
		for (auto& c : value) c ^= 0x45;
		data.value = value;
	}
}

void DataParser::read_text_file(std::filesystem::path& path, std::string& str) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("[Data Parser] Cannot open file: " + path.string());
	file.seekg(0, std::ios::end);
	size_t count = file.tellg();
	file.seekg(0, std::ios::beg);
	str.resize(count);
	file.read(str.data(), count);
}