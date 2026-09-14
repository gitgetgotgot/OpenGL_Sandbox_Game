#pragma once
#include <string>

namespace ResourceLoading {
	inline void throw_reference_error(std::string_view item_uid, std::string_view resource_uid) {
		std::string msg;
		msg.reserve(item_uid.size() + resource_uid.size() + 64);
		msg += "Load error: <";
		msg += item_uid;
		msg += ">:\nResource <";
		msg += resource_uid;
		msg += "> not found!";
		throw std::runtime_error(msg);
	}

	inline void throw_resource_error(std::string_view item_uid, std::string_view msg) {
		std::string message;
		message.reserve(item_uid.size() + msg.size() + 64);
		message += "Load error: <";
		message += item_uid;
		message += ">: Data is corrupted or incorrect:\n";
		message += msg;
		throw std::runtime_error(message);
	}

	inline bool validate_UID(std::string_view UID) {
		
		return true;
	}
};