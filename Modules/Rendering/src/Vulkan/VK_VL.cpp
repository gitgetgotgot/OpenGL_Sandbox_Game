#include "Rendering/Vulkan/VK_VL.h"

void Engine::Graphics::VK_VL::add_attribute(uint32_t layout, VertexAttributeType type, size_t stride, size_t offset) {
	elements.emplace_back(VertexAttributeElement{ layout, type, stride, offset });
	cached_stride = stride;
}

void Engine::Graphics::VK_VL::uninit() {
	elements.clear();
	cached_stride = 0;
}

VkVertexInputBindingDescription Engine::Graphics::VK_VL::get_binding_description() const {
	VkVertexInputBindingDescription binding_description{};
	binding_description.binding = 0;
	binding_description.stride = static_cast<uint32_t>(cached_stride);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return binding_description;
}

std::vector<VkVertexInputAttributeDescription> Engine::Graphics::VK_VL::get_attribute_descriptions() const {
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
	attribute_descriptions.reserve(elements.size());

	auto get_vulkan_format = [](VertexAttributeType type) {
		switch (type) {
			case VertexAttributeType::FLOAT:		return VK_FORMAT_R32_SFLOAT;
			case VertexAttributeType::FLOAT_VEC2:	return VK_FORMAT_R32G32_SFLOAT;
			case VertexAttributeType::FLOAT_VEC3:	return VK_FORMAT_R32G32B32_SFLOAT;
			case VertexAttributeType::FLOAT_VEC4:	return VK_FORMAT_R32G32B32A32_SFLOAT;

			case VertexAttributeType::UINT:			return VK_FORMAT_R32_UINT;
			case VertexAttributeType::UINT_VEC2:	return VK_FORMAT_R32G32_UINT;
			case VertexAttributeType::UINT_VEC3:	return VK_FORMAT_R32G32B32_UINT;
			case VertexAttributeType::UINT_VEC4:	return VK_FORMAT_R32G32B32A32_UINT;

			case VertexAttributeType::INT:			return VK_FORMAT_R32_SINT;
			case VertexAttributeType::INT_VEC2:		return VK_FORMAT_R32G32_SINT;
			case VertexAttributeType::INT_VEC3:		return VK_FORMAT_R32G32B32_SINT;
			case VertexAttributeType::INT_VEC4:		return VK_FORMAT_R32G32B32A32_SINT;
		}
	};

	for (const auto& elem : elements) {
		VkVertexInputAttributeDescription desc{};
		desc.binding = 0;
		desc.location = elem.location;
		desc.format = get_vulkan_format(elem.type);
		desc.offset = static_cast<uint32_t>(elem.offset);
		attribute_descriptions.push_back(desc);
	}
	return attribute_descriptions;
}