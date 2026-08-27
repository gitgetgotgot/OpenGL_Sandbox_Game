#pragma once
#include <unordered_map>
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <json/json.hpp>

enum DataNodeType : uint8_t {
	Node_NULL, Node_Value, Node_Object, Node_Array
};

class DataNode {
public:
	DataNodeType node_type = DataNodeType::Node_NULL;
	std::string value;
	std::vector<DataNode> array;
	std::unordered_map<std::string, DataNode> children;
	const DataNode& operator[](std::string key) const;
	template<typename T>
	T get_as() const;
	bool has_child(std::string name) const;
};
template<>
inline std::string DataNode::get_as<std::string>() const {
	return value;
}
template<>
inline bool DataNode::get_as<bool>() const {
	return value == "true";
}
template<>
inline int DataNode::get_as<int>() const {
	return std::stoi(value);
}
template<>
inline float DataNode::get_as<float>() const {
	return std::stof(value);
}
template<>
inline double DataNode::get_as<double>() const {
	return std::stod(value);
}
template<>
inline uint64_t DataNode::get_as<uint64_t>() const {
	return (uint64_t)std::stoull(value);
}
template<>
inline uint32_t DataNode::get_as<uint32_t>() const {
	return (uint32_t)std::stoul(value);
}
template<>
inline uint16_t DataNode::get_as<uint16_t>() const {
	return (uint16_t)std::stoul(value);
}
template<>
inline uint8_t DataNode::get_as<uint8_t>() const {
	return (uint8_t)std::stoul(value);
}

class DataParser {
public:
	DataNode node_root;
	void parse_data(std::filesystem::path& path, bool DEBUG);
	void parse_binary_format(std::filesystem::path& path);
	void parse_JSON_format(std::filesystem::path& path);
	void parse_JSON_to_binary(std::filesystem::path& path);
	void write_from_JSON(const nlohmann::json& j, DataNode& data);
	void write_JSON_to_binary(std::ofstream& out, const nlohmann::json& j);
	void write_from_binary(std::ifstream& in, DataNode& data);
};

