#pragma once
#include "Rendering/Interfaces/ShaderHandle.h"
#include <vulkan/vulkan.h>
#include <spirv_reflect.h>

namespace Engine::Graphics {
	class VK_Shader : public ShaderHandle {
	public:
		VK_Shader(
			VkDevice device,
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path,
			std::error_code& ec
		);
		~VK_Shader();
		void uninit() override;

		VkPipelineLayout get_pipeline_layout() const;
		const std::vector<VkDescriptorSetLayout>& get_descriptor_set_layouts() const;
		const std::vector<VkPipelineShaderStageCreateInfo>& get_shader_stages() const;
		void free_shader_modules();
	private:
		struct DescriptorBindingInfo {
			uint32_t binding;
			VkDescriptorType descriptor_type;
			uint32_t count;
			VkShaderStageFlags stage_flags;
		};
	private:
		VkShaderModule create_shader_module(const std::vector<uint32_t>& code, std::error_code& ec) const;
		void create_pipeline_layout(
			const SpvReflectShaderModule& vert_module,
			const SpvReflectShaderModule& frag_module,
			std::error_code& ec
		);

		VkDevice device = VK_NULL_HANDLE;
		VkShaderModule vert_shader_module = VK_NULL_HANDLE;
		VkShaderModule frag_shader_module = VK_NULL_HANDLE;

		VkPipelineLayout pipeline_layout{ VK_NULL_HANDLE };
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
		mutable std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};
}