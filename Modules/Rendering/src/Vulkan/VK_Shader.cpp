#include "Rendering/Vulkan/VK_Shader.h"
#include <Base/EngineError.h>
#include <map>

Engine::Graphics::VK_Shader::VK_Shader(
	VkDevice device,
	const std::filesystem::path& vertex_shader_path,
	const std::filesystem::path& fragment_shader_path,
    std::error_code& ec
) : device{ device } {
    // read byte-code (SPIR-V) of both shaders
    auto vert_code = read_file_uint32_t(vertex_shader_path, ec);
    if (ec) return;
    vert_shader_module = create_shader_module(vert_code, ec);
    if (ec) return;

    auto frag_code = read_file_uint32_t(fragment_shader_path, ec);
    if (ec) return;
    frag_shader_module = create_shader_module(frag_code, ec);
    if (ec) return;

    // Automatic reflect with spirv-reflect for both vert and frag
    SpvReflectShaderModule vert_reflect_module{};
    SpvReflectResult result = spvReflectCreateShaderModule(
        vert_code.size() * sizeof(uint32_t),
        vert_code.data(),
        &vert_reflect_module
    );
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        ec = Engine::Errors::EngineError::ShaderSpirvReflectError;
        return;
    }
    SpvReflectShaderModule frag_reflect_module{};
    result = spvReflectCreateShaderModule(
        frag_code.size() * sizeof(uint32_t),
        frag_code.data(),
        &frag_reflect_module
    );
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        spvReflectDestroyShaderModule(&vert_reflect_module);
        ec = Engine::Errors::EngineError::ShaderSpirvReflectError;
        return;
    }

    // create Pipeline Layout for these shaders
    create_pipeline_layout(vert_reflect_module, frag_reflect_module, ec);
    if (ec) return;

    // clear reflect data
    spvReflectDestroyShaderModule(&vert_reflect_module);
    spvReflectDestroyShaderModule(&frag_reflect_module);

    // create structures for pipeline shader stages
    VkPipelineShaderStageCreateInfo vert_stage_info{};
    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = vert_shader_module;
    vert_stage_info.pName = "main"; // usual name of MAIN function in shader
    shader_stages.push_back(vert_stage_info);

    VkPipelineShaderStageCreateInfo frag_stage_info{};
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = frag_shader_module;
    frag_stage_info.pName = "main";
    shader_stages.push_back(frag_stage_info);
}

Engine::Graphics::VK_Shader::~VK_Shader() {
    uninit();
}

void Engine::Graphics::VK_Shader::uninit() {
    free_shader_temporary_data();
    if (pipeline_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        pipeline_layout = VK_NULL_HANDLE;
    }
    for (auto& ds : descriptor_set_layouts) {
        if (ds != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(device, ds, nullptr);
            ds = VK_NULL_HANDLE;
        }
    }
}

void Engine::Graphics::VK_Shader::free_shader_temporary_data() {
    if (vert_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, vert_shader_module, nullptr);
        vert_shader_module = VK_NULL_HANDLE;
    }
    if (frag_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, frag_shader_module, nullptr);
        frag_shader_module = VK_NULL_HANDLE;
    }
    descriptor_pool_sizes.clear();
}

VkShaderModule Engine::Graphics::VK_Shader::create_shader_module(const std::vector<uint32_t>& code, std::error_code& ec) const {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size() * sizeof(uint32_t); // size in bytes
    create_info.pCode = code.data();

    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanShaderCreateError;
        return {};
    }
    return shader_module;
}

void Engine::Graphics::VK_Shader::create_pipeline_layout(
    const SpvReflectShaderModule& vert_module,
    const SpvReflectShaderModule& frag_module,
    std::error_code& ec
) {
    // ====== 1. Get Descriptor Sets for shaders ======
    // SetIndex -> (BindingIndex -> BindingInfo)
    std::map<uint32_t, std::map<uint32_t, DescriptorBindingInfo>> sets_map;

    auto add_shader_bindings = [&](const SpvReflectShaderModule& module, VkShaderStageFlagBits stage_flag) {
        uint32_t count = 0;
        spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
        std::vector<SpvReflectDescriptorSet*> sets(count);
        spvReflectEnumerateDescriptorSets(&module, &count, sets.data());

        for (const auto* set : sets) {
            for (uint32_t i = 0; i < set->binding_count; ++i) {
                const auto* ref_binding = set->bindings[i];
                auto& binding_info = sets_map[set->set][ref_binding->binding];

                // if binding already exist (f.e. in other shader), then update flags
                if (binding_info.stage_flags != 0) {
                    binding_info.stage_flags |= stage_flag;
                }
                else {
                    binding_info.binding = ref_binding->binding;
                    binding_info.descriptor_type = static_cast<VkDescriptorType>(ref_binding->descriptor_type);
                    binding_info.count = ref_binding->count;
                    binding_info.stage_flags = stage_flag;
                }
            }
        }
    };

    add_shader_bindings(vert_module, VK_SHADER_STAGE_VERTEX_BIT);
    add_shader_bindings(frag_module, VK_SHADER_STAGE_FRAGMENT_BIT);

    // translate sets map to VkDescriptorSetLayout
    // also setup map for total count of all descriptor types
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
    std::map<VkDescriptorType, uint32_t> type_counts;

    for (const auto& [set_index, bindings_map] : sets_map) {
        
        std::vector<VkDescriptorSetLayoutBinding> vk_bindings;

        for (const auto& [binding_index, info] : bindings_map) {
            VkDescriptorSetLayoutBinding vk_binding{};
            vk_binding.binding = info.binding;
            vk_binding.descriptorType = info.descriptor_type;
            vk_binding.descriptorCount = info.count;
            vk_binding.stageFlags = info.stage_flags;
            vk_binding.pImmutableSamplers = nullptr;

            vk_bindings.push_back(vk_binding);

            type_counts[info.descriptor_type] += info.count;
        }

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(vk_bindings.size());
        layout_info.pBindings = vk_bindings.data();

        VkDescriptorSetLayout set_layout;
        if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &set_layout) != VK_SUCCESS) {
            ec = Engine::Errors::EngineError::VulkanDescriptorSetLayoutError;
            return;
        }
        descriptor_set_layouts.push_back(set_layout);
    }
    this->descriptor_set_layouts = descriptor_set_layouts;

    // setup vector for descriptor pool sizes
    for (const auto& [type, count] : type_counts) {
        VkDescriptorPoolSize pool_size{};
        pool_size.type = type;
        pool_size.descriptorCount = count;
        descriptor_pool_sizes.push_back(pool_size);
    }

    // ====== 2. Get Push Constants for shaders ======
    // map key: pair {offset, size}, value: stage flags
    std::map<std::pair<uint32_t, uint32_t>, VkShaderStageFlags> push_constants_map;

    auto add_shader_push_constants = [&](const SpvReflectShaderModule& module, VkShaderStageFlagBits stage_flag) {
        uint32_t count = 0;
        spvReflectEnumeratePushConstantBlocks(&module, &count, nullptr);
        if (count > 0) {
            std::vector<SpvReflectBlockVariable*> blocks(count);
            spvReflectEnumeratePushConstantBlocks(&module, &count, blocks.data());
            for (const auto* block : blocks) {
                auto key = std::make_pair(block->offset, block->size);
                push_constants_map[key] |= stage_flag;
            }
        }
    };

    add_shader_push_constants(vert_module, VK_SHADER_STAGE_VERTEX_BIT);
    add_shader_push_constants(frag_module, VK_SHADER_STAGE_FRAGMENT_BIT);

    // Convert to Vulkan format
    std::vector<VkPushConstantRange> vk_push_constant_ranges;
    for (const auto& [range_key, stage_flags] : push_constants_map) {
        VkPushConstantRange pc_range{};
        pc_range.offset = range_key.first;
        pc_range.size = range_key.second;
        pc_range.stageFlags = stage_flags; // automatic apply (VERTEX, FRAGMENT or both)
        vk_push_constant_ranges.push_back(pc_range);
    }

    // ====== 3. Create Pipeline Layuot ======
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(vk_push_constant_ranges.size());
    pipeline_layout_info.pPushConstantRanges = vk_push_constant_ranges.data();

    if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &this->pipeline_layout) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanPipelineLayoutError;
        return;
    }
}