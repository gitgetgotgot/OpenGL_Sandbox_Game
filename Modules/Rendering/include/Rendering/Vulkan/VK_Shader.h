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

		VkPipelineLayout get_pipeline_layout() const { return pipeline_layout; }
		const std::vector<VkDescriptorSetLayout>& get_descriptor_set_layouts() const { return descriptor_set_layouts; }
		const std::vector<VkDescriptorPoolSize>& get_descriptor_pool_sizes() const { return descriptor_pool_sizes; }
		const std::vector<VkPipelineShaderStageCreateInfo>& get_shader_stages() const { return shader_stages; }
		void free_shader_temporary_data();
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

		// shader modules (vert and frag) are wrappers of shader code
		VkShaderModule vert_shader_module = VK_NULL_HANDLE;
		VkShaderModule frag_shader_module = VK_NULL_HANDLE;

		// global "template" of shader interface
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

		// layouts for different descriptor sets for this pair of shaders (vert and frag),
		// but for now only default set is used for all shaders (set = 0)
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts;

		// descriptor pool sizes to create VkDescriptorPool in Pipeline
		std::vector<VkDescriptorPoolSize> descriptor_pool_sizes;

		// shader stages (vert and frag), that connect shader modules with pipeline stages and set main "enter points"
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};
}