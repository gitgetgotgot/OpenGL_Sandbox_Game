#pragma once
#include "Rendering/Interfaces/VertexLayout.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine::Graphics {
	class VK_VL : public VertexLayout {
	public:
		VK_VL() = default;
		~VK_VL() = default;
		void add_attribute(uint32_t layout, VertexAttributeType type, size_t stride, size_t offset) const override;
		void bind() const override {};
		void unbind() const override {};
		void uninit() const override { elements.clear(); }
		VkVertexInputBindingDescription get_binding_description() const;
		std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions() const;
	private:
		mutable std::vector<VertexAttributeElement> elements;
		mutable size_t cached_stride = 0;
	};
}