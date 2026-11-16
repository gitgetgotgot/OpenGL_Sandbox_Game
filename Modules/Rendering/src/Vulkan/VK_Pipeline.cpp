#include "Rendering/Vulkan/VK_Pipeline.h"
#include "Rendering/Vulkan/VK_Shader.h"
#include "Rendering/Vulkan/VK_VL.h"
#include "Rendering/Vulkan/VK_UB.h"
#include "Rendering/Vulkan/VK_SB.h"
#include <Base/EngineError.h>

Engine::Graphics::VK_Pipeline::VK_Pipeline(VkDevice device, VkRenderPass render_pass) :
	device{ device }, render_pass{ render_pass } {
}

Engine::Graphics::VK_Pipeline::~VK_Pipeline() {
	uninit();
}

void Engine::Graphics::VK_Pipeline::init(
	const ShaderHandle& shader,
	const VertexLayout& layout,
    PipelinePrimitives p,
	PipelineScissor s,
	PipelineViewport v,
	PipelineBlendMode bm,
	PipelineDepthMode dm,
	PipelineCullMode cm,
    std::error_code& ec
) {
    // 1. Get shader stages from shader
    auto& vulkan_shader = static_cast<const Engine::Graphics::VK_Shader&>(shader);
    auto& vulkan_shader_stages = vulkan_shader.get_shader_stages();

    // 2. Get Vertex Layout from layout
    auto& vulkan_VL = static_cast<const Engine::Graphics::VK_VL&>(layout);
    auto binding_desc = vulkan_VL.get_binding_description();
    auto attribute_descs = vulkan_VL.get_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input{};
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input.vertexBindingDescriptionCount = 1;
    vertex_input.pVertexBindingDescriptions = &binding_desc;
    vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descs.size());
    vertex_input.pVertexAttributeDescriptions = attribute_descs.data();

    // 3. Topology of primitives (Point, Line or Triangle)
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    if (p == PipelinePrimitives::Line) topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if (p == PipelinePrimitives::Triangle) topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = topology;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    // 4. Viewport and Scissor Static/Dynamic
    std::vector<VkDynamicState> dynamic_states;
    if (v == PipelineViewport::Dynamic) dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    else if (s == PipelineScissor::Dynamic) dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    // Если они статические, сюда нужно передать указатели на конкретные VkViewport / VkRect2D, 
    // но для UI мы используем Dynamic, поэтому структуры могут оставаться пустыми (но валидными).

    // 5. Rasterization and CullMode
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    // CullMode
    switch (cm) {
        case PipelineCullMode::None:  rasterizer.cullMode = VK_CULL_MODE_NONE; break;
        case PipelineCullMode::Back:  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; break;
        case PipelineCullMode::Front: rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT; break;
    }
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // 6. Multisampling (OFF for now)
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // 7. Depth / Stencil mode (PipelineDepthMode)
    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    switch (dm) {
    case PipelineDepthMode::None:
        depth_stencil.depthTestEnable = VK_FALSE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        break;
    case PipelineDepthMode::ReadAndWrite:
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        break;
    case PipelineDepthMode::Readonly:
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_FALSE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        break;
    }

    // 8. Blend Mode (PipelineBlendMode)
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    if (bm == PipelineBlendMode::None) {
        color_blend_attachment.blendEnable = VK_FALSE;
    }
    else {
        color_blend_attachment.blendEnable = VK_TRUE;
        if (bm == PipelineBlendMode::Alpha) {
            color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
        else if (bm == PipelineBlendMode::Additive) {
            color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }
    }

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    // 9. Pipeline Info
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = vulkan_shader_stages.data(); // shader stages (vert & frag)
    pipeline_info.pVertexInputState = &vertex_input;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = vulkan_shader.get_pipeline_layout(); // Pipeline Layout of shader (Descriptor Sets & Push Constants)
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;

    // create pipeline
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->pipeline) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanPipelineCreateError;
        return;
    }
    this->pipeline_layout = vulkan_shader.get_pipeline_layout(); // remember pipeline layout

    // 10. Descriptor pool and descriptor sets
    // create descriptor pool to use descriptors
    const auto& pool_sizes = vulkan_shader.get_descriptor_pool_sizes();

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.maxSets = static_cast<uint32_t>(vulkan_shader.get_descriptor_set_layouts().size());
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanDescriptorPoolError;
        return;
    }

    // allocate descriptor sets
    const auto& set_layouts = vulkan_shader.get_descriptor_set_layouts();
    if (!set_layouts.empty()) {
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(set_layouts.size());
        alloc_info.pSetLayouts = set_layouts.data();

        descriptor_sets.resize(set_layouts.size());
        if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets.data()) != VK_SUCCESS) {
            ec = Engine::Errors::EngineError::VulkanDescriptorSetsAllocError;
            return;
        }
    }
}

void Engine::Graphics::VK_Pipeline::uninit() {
    // destroy pipeline handle
	if (pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(device, pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;
	}
    // destroy descriptor pool (descriptor sets are destroyed automatically)
    if (descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
        descriptor_pool = VK_NULL_HANDLE;
        descriptor_sets.clear();
    }
}

void Engine::Graphics::VK_Pipeline::bind_uniform_buffer(const UniformBuffer& ub, uint32_t binding, uint32_t set = 0) {
    const auto& vk_ub = static_cast<const VK_UB&>(ub);

    VkDescriptorBufferInfo buffer_info = vk_ub.get_descriptor_info();

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = descriptor_sets[set];
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(device, 1, &descriptor_write, 0, nullptr);
}

void Engine::Graphics::VK_Pipeline::bind_storage_buffer(const StorageBuffer& sb, uint32_t binding, uint32_t set = 0) {
    const auto& vk_sb = static_cast<const VK_SB&>(sb);

    VkDescriptorBufferInfo buffer_info = vk_sb.get_descriptor_info();

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = descriptor_sets[set];
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(device, 1, &descriptor_write, 0, nullptr);
}

void Engine::Graphics::VK_Pipeline::bind_sampler_2D(const Texture& tex, uint32_t binding = 0, uint32_t set = 0) {

}

void Engine::Graphics::VK_Pipeline::bind_sampler_2D_array(const TextureArray& tex_arr, uint32_t binding = 0, uint32_t set = 0) {

}