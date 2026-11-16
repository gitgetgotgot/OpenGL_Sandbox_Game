#include "Rendering/Vulkan/VK_VB.h"

Engine::Graphics::VK_VB::VK_VB(
    VkDevice device, VmaAllocator allocator, VkCommandPool command_pool, VkQueue graphics_queue
) :
    device{ device }, allocator{ allocator }, command_pool{ command_pool }, graphics_queue{ graphics_queue } {
}

Engine::Graphics::VK_VB::~VK_VB() {
	uninit();
}

void Engine::Graphics::VK_VB::set_data(const void* data, size_t size, VertexBufUsage usage) {
    uninit();
    buffer_size = size;
    buffer_usage = usage;

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    // static mode -> allow data copy through GPU commands
    if (usage == VertexBufUsage::USAGE_STATIC) {
        buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};

    if (usage == VertexBufUsage::USAGE_DYNAMIC) {
        // Dynamic mode -> direct access to buffer memory through CPU, memory is mapped to void* ptr for fast std::memcpy
        // (good for constant updates)
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
        // Static mode -> store buffer in VRAM without direct access for CPU
        // (good for const data or rare updates)
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr);

        if (data != nullptr) {
            send_data_to_static_buffer(data, size, 0);
        }
    }
}

void Engine::Graphics::VK_VB::update_data(const void* data, size_t size, size_t offset) const {
    if (buffer == VK_NULL_HANDLE || mapped_data == nullptr) return;

    if (buffer_usage == VertexBufUsage::USAGE_DYNAMIC) {
        if (offset + size <= buffer_size) {
            std::memcpy(static_cast<char*>(mapped_data) + offset, data, size);
        }
    }
    else {
        send_data_to_static_buffer(data, size, offset);
    }
}

void Engine::Graphics::VK_VB::uninit() {
    if (buffer != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE) {
        vmaDestroyBuffer(allocator, buffer, allocation);
        buffer = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
        mapped_data = nullptr;
        buffer_size = 0;
    }
}

VkBuffer Engine::Graphics::VK_VB::get_buffer() const {
    return buffer;
}

void Engine::Graphics::VK_VB::send_data_to_static_buffer(const void* data, size_t size, size_t offset) const {
    // create temporary staging buffer in a Host-Visible memory
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
        &staging_buffer, &staging_allocation, &staging_alloc_result);

    // copy data to this buffer
    std::memcpy(staging_alloc_result.pMappedData, data, size);

    // create command to copy buffer through temporary command buffer
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

    // send command buffer to execute command
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer;

    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue); // wait for queue to be completed

    // destroy staging buffer and command buffer
    vkFreeCommandBuffers(device, command_pool, 1, &cmd_buffer);
    vmaDestroyBuffer(allocator, staging_buffer, staging_allocation);
}