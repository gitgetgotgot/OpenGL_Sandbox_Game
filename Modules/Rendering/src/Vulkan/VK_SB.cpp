#include "Rendering/Vulkan/VK_SB.h"
#include <cstring>

Engine::Graphics::VK_SB::VK_SB(
    VkDevice device, VmaAllocator allocator, VkCommandPool command_pool, VkQueue graphics_queue
) :
    device{ device }, allocator{ allocator }, command_pool{ command_pool }, graphics_queue{ graphics_queue } {
}

Engine::Graphics::VK_SB::~VK_SB() {
    uninit();
}

void Engine::Graphics::VK_SB::set_data(const void* data, size_t size, StorageBufUsage usage) {
    uninit();
    buffer_size = size;
    buffer_usage = usage;

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    if (usage == StorageBufUsage::USAGE_STATIC) {
        buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};

    if (usage == StorageBufUsage::USAGE_DYNAMIC) {
        alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocationInfo alloc_result_info{};
        vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, &alloc_result_info);

        mapped_data = alloc_result_info.pMappedData;
        if (data != nullptr && mapped_data != nullptr) {
            std::memcpy(mapped_data, data, size);
        }
    }
    else {
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr);

        if (data != nullptr) {
            send_data_to_static_buffer(data, size, 0);
        }
    }
}

void Engine::Graphics::VK_SB::update_data(const void* data, size_t size, size_t offset) const {
    if (buffer == VK_NULL_HANDLE || mapped_data == nullptr) return;

    if (buffer_usage == StorageBufUsage::USAGE_DYNAMIC) {
        if (offset + size <= buffer_size) {
            std::memcpy(static_cast<char*>(mapped_data) + offset, data, size);
        }
    }
    else {
        send_data_to_static_buffer(data, size, offset);
    }
}

void Engine::Graphics::VK_SB::uninit() {
    if (buffer != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE) {
        vmaDestroyBuffer(allocator, buffer, allocation);
        buffer = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
        mapped_data = nullptr;
        buffer_size = 0;
    }
}

VkDescriptorBufferInfo Engine::Graphics::VK_SB::get_descriptor_info() const {
    VkDescriptorBufferInfo info{};
    info.buffer = buffer;
    info.offset = 0;
    info.range = buffer_size;
    return info;
}

void Engine::Graphics::VK_SB::send_data_to_static_buffer(const void* data, size_t size, size_t offset) const {
    VkBufferCreateInfo staging_buffer_info{};
    staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    staging_buffer_info.size = size;
    staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo staging_alloc_info{};
    staging_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    staging_alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer staging_buffer;
    VmaAllocation staging_allocation;
    VmaAllocationInfo staging_alloc_result{};

    vmaCreateBuffer(allocator, &staging_buffer_info, &staging_alloc_info,
        &staging_buffer, &staging_allocation, &staging_alloc_result
    );
    std::memcpy(staging_alloc_result.pMappedData, data, size);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer cmd_buffer;
    vkAllocateCommandBuffers(device, &alloc_info, &cmd_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = offset;
    copy_region.size = size;
    vkCmdCopyBuffer(cmd_buffer, staging_buffer, buffer, 1, &copy_region);
    vkEndCommandBuffer(cmd_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer;

    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &cmd_buffer);
    vmaDestroyBuffer(allocator, staging_buffer, staging_allocation);
}