#include "Resources/TagRegistry.h"

void Engine::Resource::Tag::TagRegistry::ClearData() {
	current_tag_index = 0;
	UID_to_ID = {};
}

uint16_t Engine::Resource::Tag::TagRegistry::add_tag(std::string_view tag_name) {
	UID_to_ID.emplace(tag_name, current_tag_index);
	return current_tag_index++;
}

std::optional<uint16_t> Engine::Resource::Tag::TagRegistry::get_tag(std::string tag_name) {
	auto it = UID_to_ID.find(tag_name);
	if (it == UID_to_ID.end()) return std::nullopt;
	else return it->second;
}