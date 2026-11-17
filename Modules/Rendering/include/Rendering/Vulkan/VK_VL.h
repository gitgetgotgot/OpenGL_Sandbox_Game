#pragma once
#include "Rendering/RHI/VertexLayout.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine::Graphics {
	class VK_VL : public VertexLayout {
	public:
		VK_VL() = default;
		~VK_VL() = default;
		void add_attribute(uint32_t layout, VertexAttributeType type, size_t stride, size_t offset) override;
		void uninit() override;

		VkVertexInputBindingDescription get_binding_description() const;
		std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions() const;
	private:
		std::vector<VertexAttributeElement> elements;
		size_t cached_stride = 0;
	};
}