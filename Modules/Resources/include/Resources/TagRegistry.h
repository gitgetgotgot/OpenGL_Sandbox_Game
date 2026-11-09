#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

namespace Engine::Resource::Tag {
	class TagRegistry {
	public:
		static TagRegistry& Instance() {
			static TagRegistry registry;
			return registry;
		}
		void ClearData();
		uint16_t add_tag(const std::string& tag_name);
		std::optional<uint16_t> get_tag(const std::string& tag_name);
	private:
		TagRegistry() {}
		~TagRegistry() {}
		uint16_t current_tag_index = 0;
		std::unordered_map<std::string, uint16_t> UID_to_ID;
	};
}