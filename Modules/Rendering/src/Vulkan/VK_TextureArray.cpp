#include "Rendering/Vulkan/VK_TextureArray.h"
#include <Base/EngineError.h>
#include <stb_image.h>

Engine::Graphics::VK_TextureArray::VK_TextureArray(VkDevice device, VmaAllocator allocator, VkCommandPool command_pool, VkQueue graphics_queue) :
	device{ device }, allocator{ allocator }, command_pool{ command_pool }, graphics_queue{ graphics_queue } {
}

Engine::Graphics::VK_TextureArray::~VK_TextureArray() {
    if (sampler != VK_NULL_HANDLE) {
        vkDestroySampler(device, sampler, nullptr);
        sampler = VK_NULL_HANDLE;
    }
    if (image_view != VK_NULL_HANDLE) {
        vkDestroyImageView(device, image_view, nullptr);
        image_view = VK_NULL_HANDLE;
    }
    if (allocation != VK_NULL_HANDLE) {
        vmaDestroyImage(allocator, image, allocation);
        allocation = VK_NULL_HANDLE;
        image = VK_NULL_HANDLE;
    }
}

void Engine::Graphics::VK_TextureArray::setup_texture_array(
	uint32_t width, uint32_t height, uint32_t depth, FilterMode filter, WrapMode wrap,
	uint8_t channels, ResizeStrategy resize_str, std::error_code& ec
) {
    this->width = width;
    this->height = height;
    this->filter = filter;
    this->wrap = wrap;
    this->resize_strategy = resize_str;
    this->channels = channels;
    uint32_t new_depth = depth;

    VkImage new_image = VK_NULL_HANDLE;
    VmaAllocation new_allocation = VK_NULL_HANDLE;
    VkImageView new_image_view = VK_NULL_HANDLE;

    create_tex_array(new_image, new_allocation, new_image_view, new_depth, ec);
    if (ec) return;

    image = new_image;
    allocation = new_allocation;
    image_view = new_image_view;
    this->depth = new_depth;

    // Create sampler after Image, Allocation and ImageView were created
    VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    if (wrap == WrapMode::Repeat)              address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    else if (wrap == WrapMode::MirroredRepeat) address_mode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    else if (wrap == WrapMode::ClampToBorder)  address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

    VkFilter vk_filter = (filter == FilterMode::Linear) ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = vk_filter;
    sampler_info.minFilter = vk_filter;
    sampler_info.mipmapMode = (filter == FilterMode::Linear) ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler_info.addressModeU = address_mode;
    sampler_info.addressModeV = address_mode;
    sampler_info.addressModeW = address_mode;
    sampler_info.mipLodBias = 0.0f;
    // anisotropy filtration (usually turned off for 2D, especially with Nearest filter)
    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.maxAnisotropy = 1.0f;
    // compare depth (needed only for shadow maps, so it's turned off here)
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    // range of mip levels
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f; // or VK_LOD_CLAMP_NONE, if mipmaps occur
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE; // use standard UV [0.0, 1.0]

    if (vkCreateSampler(device, &sampler_info, nullptr, &sampler) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanTexSamplerCreateError;
        return;
    }

    // Create temporary cmd buf and process image layout change
    VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
    create_and_begin_temp_cmd_buffer(cmd_buf);

    // change image layers layout from "undefined" to "shader read" state
    change_image_layer_layouts(
        image, cmd_buf,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        0, depth
    );

    submit_and_destroy_temp_cmd_buffer(cmd_buf);
}

void Engine::Graphics::VK_TextureArray::add_2D_texture(const char* fileName, std::error_code& ec) {
    int imgWidth, imgHeight, numOfColorChannels;
    int desired_channels = (channels == 3) ? 4 : static_cast<int>(channels); //convert RGB to RGBA

    unsigned char* image_bytes = stbi_load(fileName, &imgWidth, &imgHeight, &numOfColorChannels, desired_channels);

    if (!image_bytes) {
        //ec
        return;
    }
    if (imgWidth != width || imgHeight != height) {
        stbi_image_free(image_bytes);
        //ec
        return;
    }

    add_2D_texture(image_bytes, imgWidth, imgHeight, desired_channels, ec);
    stbi_image_free(image_bytes);
}

void Engine::Graphics::VK_TextureArray::add_2D_texture(const unsigned char* img_buf, int width, int height, int numOfChannels, std::error_code& ec) {
    // img buf, width, height and numberOfChannels are checked in textures manager
    if (current_size == depth) resize_array(ec);
    if (ec) return;

    // 1. Create Host-Visible Staging Buffer through VMA
    VkDeviceSize image_size = static_cast<VkDeviceSize>(width) * height * numOfChannels; // image size in bytes

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = image_size;
    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo staging_alloc_info{};
    staging_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    staging_alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
        VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer staging_buffer = VK_NULL_HANDLE;
    VmaAllocation staging_allocation = VK_NULL_HANDLE;
    VmaAllocationInfo alloc_details{};

    if (vmaCreateBuffer(allocator, &buffer_info, &staging_alloc_info, &staging_buffer, &staging_allocation, &alloc_details) != VK_SUCCESS) {
        // ec ..
        return;
    }

    // Copy pixels to staging buffer (memory is mapped by VMA_ALLOCATION_CREATE_MAPPED_BIT)
    std::memcpy(alloc_details.pMappedData, img_buf, static_cast<size_t>(image_size));

    // 2. Create temporary command buffer
    VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
    create_and_begin_temp_cmd_buffer(cmd_buf);

    // 3. Barrier #1: convert current layer from SHADER_READ_ONLY_OPTIMAL to TRANSFER_DST_OPTIMAL to send data
    change_image_layer_layouts(image, cmd_buf,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        current_size, 1
    );

    // 4. Copy data from staging buffer to needed layer in VkImage
    VkBufferImageCopy copy_region{};
    copy_region.bufferOffset = 0;
    copy_region.bufferRowLength = 0;   // dense packing of pixels without gaps
    copy_region.bufferImageHeight = 0;
    copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.imageSubresource.mipLevel = 0;
    copy_region.imageSubresource.baseArrayLayer = current_size; // array index
    copy_region.imageSubresource.layerCount = 1;
    copy_region.imageOffset = { 0, 0, 0 };
    copy_region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };

    vkCmdCopyBufferToImage(
        cmd_buf,
        staging_buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copy_region
    );

    // 5. Barrier #2: convert current layer from TRANSFER_DST_OPTIMAL back to SHADER_READ_ONLY_OPTIMAL to read only
    change_image_layer_layouts(image, cmd_buf,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        current_size, 1
    );

    // 6. Submit commands, then wait for completion and destroy command buffer
    submit_and_destroy_temp_cmd_buffer(cmd_buf);

    // 7. Clear temporary stage buffer
    vmaDestroyBuffer(allocator, staging_buffer, staging_allocation);

    ++current_size;
}

VkDescriptorImageInfo Engine::Graphics::VK_TextureArray::get_descriptor_info() const {
    VkDescriptorImageInfo info{};
    info.sampler = sampler;
    info.imageView = image_view;
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    return info;
}

void Engine::Graphics::VK_TextureArray::create_tex_array(
    VkImage& new_image, VmaAllocation& new_allocation, VkImageView& new_image_view, uint32_t new_depth, std::error_code& ec
) {
    VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;
    if (channels == 1)      image_format = VK_FORMAT_R8_SRGB;
    else if (channels == 2) image_format = VK_FORMAT_R8G8_SRGB;
    else                    image_format = VK_FORMAT_R8G8B8A8_SRGB; // if RGB then use RGBA

    // 1. create image info
    VkImageCreateInfo img_info{};
    img_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    img_info.imageType = VK_IMAGE_TYPE_2D;
    img_info.extent = VkExtent3D{ width, height, 1 };
    img_info.mipLevels = 1; // no mipmaps
    img_info.arrayLayers = new_depth;
    img_info.format = image_format;
    img_info.tiling = VK_IMAGE_TILING_OPTIMAL; // optimal position in VRAM for fast access
    img_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // initial state after creation
    img_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | // copy from staging buffer
        VK_IMAGE_USAGE_SAMPLED_BIT |      // read in shader
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT;  // copy for array resize
    img_info.samples = VK_SAMPLE_COUNT_1_BIT; // no multisampling
    img_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // used by single queue (Graphics)

    // 2. allocate memory and create image
    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // allocate in VRAM

    if (vmaCreateImage(allocator, &img_info, &alloc_info, &new_image, &new_allocation, nullptr) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanTexImageCreateError;
        return;
    }

    // 3. create image view info
    VkImageViewCreateInfo img_view_info{};
    img_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    img_view_info.image = new_image;
    img_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    img_view_info.format = image_format;
    // channels mapping default
    img_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    img_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    img_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    img_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // set layers and mip levels
    img_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    img_view_info.subresourceRange.baseMipLevel = 0;
    img_view_info.subresourceRange.levelCount = 1;
    img_view_info.subresourceRange.baseArrayLayer = 0;
    img_view_info.subresourceRange.layerCount = new_depth;

    if (vkCreateImageView(device, &img_view_info, nullptr, &new_image_view) != VK_SUCCESS) {
        ec = Engine::Errors::EngineError::VulkanTexImageViewCreateError;
        return;
    }
}

void Engine::Graphics::VK_TextureArray::resize_array(std::error_code& ec) {
    // 1. Resize new depth
    uint32_t new_depth = depth;

    switch (resize_strategy) {
        case ResizeStrategy::Resize_by_100_percent: new_depth = (depth == 0) ? 2 : depth * 2; break;
        case ResizeStrategy::Resize_by_50_percent:  new_depth = (depth < 2) ? 2 : depth + depth / 2; break;
        case ResizeStrategy::Resize_by_1:           new_depth += 1; break;
        case ResizeStrategy::Resize_by_2:           new_depth += 2; break;
        case ResizeStrategy::Resize_by_5:           new_depth += 5; break;
    }

    // 2. Create new texture array
    VkImage new_image = VK_NULL_HANDLE;
    VmaAllocation new_allocation = VK_NULL_HANDLE;
    VkImageView new_image_view = VK_NULL_HANDLE;

    create_tex_array(new_image, new_allocation, new_image_view, new_depth, ec);
    if (ec) return;

    // 3. Create temporary command buffer and begin
    VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
    create_and_begin_temp_cmd_buffer(cmd_buf);

    // 4. Change old layout to "transfer source" and new layout to "transfer destination"
    change_image_layer_layouts(
        image, cmd_buf,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        0, depth
    );
    change_image_layer_layouts(
        new_image, cmd_buf,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0, new_depth
    );

    // 5. Copy old image data to new image
    if (current_size > 0) {
        VkImageCopy copy_region{};
        copy_region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, current_size };
        copy_region.srcOffset = { 0, 0, 0 };
        copy_region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, current_size };
        copy_region.dstOffset = { 0, 0, 0 };
        copy_region.extent = { width, height, 1 };

        vkCmdCopyImage(
            cmd_buf,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            new_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &copy_region
        );
    }

    // 6. Change new layout to "shader read"
    change_image_layer_layouts(
        new_image, cmd_buf,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        0, new_depth
    );

    // 7. Submit all commands and destroy temp cmd buf
    submit_and_destroy_temp_cmd_buffer(cmd_buf);

    // 8. Destroy old texture array (ImageView, Image & Allocation)
    vkDestroyImageView(device, image_view, nullptr);
    vmaDestroyImage(allocator, image, allocation);

    // 9. Remember new objects
    depth = new_depth;
    image = new_image;
    allocation = new_allocation;
    image_view = new_image_view;

    mark_as_dirty();
}

void Engine::Graphics::VK_TextureArray::create_and_begin_temp_cmd_buffer(VkCommandBuffer& cmd_buf) const {
    // allocate temporary commands buffer and begin it as one time submit buffer
    VkCommandBufferAllocateInfo cmd_alloc_info{};
    cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_alloc_info.commandPool = command_pool;
    cmd_alloc_info.commandBufferCount = 1;

    cmd_buf = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(device, &cmd_alloc_info, &cmd_buf);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd_buf, &begin_info);
}

void Engine::Graphics::VK_TextureArray::submit_and_destroy_temp_cmd_buffer(VkCommandBuffer& cmd_buf) const {
    // end buffer, submit, wait to complete commands, destroy
    vkEndCommandBuffer(cmd_buf);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf;

    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);
    vkFreeCommandBuffers(device, command_pool, 1, &cmd_buf);
}

void Engine::Graphics::VK_TextureArray::change_image_layer_layouts(
    VkImage& target_image, VkCommandBuffer& cmd_buf,
    VkImageLayout old_layout, VkImageLayout new_layout,
    uint32_t base_layer, uint32_t layer_count
) const {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = target_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = base_layer;
    barrier.subresourceRange.layerCount = layer_count;

    VkPipelineStageFlags src_stage = 0;
    VkPipelineStageFlags dst_stage = 0;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier.srcAccessMask = 0;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    if (new_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        barrier.dstAccessMask = 0;
        dst_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    else if (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    vkCmdPipelineBarrier(
        cmd_buf,
        src_stage, dst_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

void Engine::Graphics::VK_TextureArray::mark_as_dirty() {
    // call renderer
}