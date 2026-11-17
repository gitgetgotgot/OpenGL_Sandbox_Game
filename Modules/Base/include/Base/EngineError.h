#pragma once
#include <system_error>
#include <string>
#include <cstdint>

namespace Engine::Errors {
    enum class EngineError : uint8_t {
        Success = 0,

        // Shader file
        ShaderFileLoadError,
        ShaderSpirvReflectError,

        // Vulkan errors
        VulkanDeviceNotFound,
        VulkanNotSupported,
        VulkanOutOfMemory,
        VulkanLogicalDeviceInitError,
        VulkanSwapChainInitError,
        VulkanImageViewInitError,
        VulkanRenderPassInitError,
        VulkanFramebufferInitError,
        VulkanCommandPoolInitError,
        VulkanCommandBufferAllocError,
        VulkanSyncObjectsInitError,

        VulkanFenceWaitError,
        VulkanAcquireImageError,
        VulkanFenceResetError,
        VulkanCommandBufferBeginError,
        VulkanCommandBufferEndError,
        VulkanQueueSubmitError,
        VulkanPresentError,

        VulkanShaderCreateError,
        VulkanPipelineCreateError,
        VulkanDescriptorSetLayoutError,
        VulkanPipelineLayoutError,
        VulkanDescriptorPoolError,
        VulkanDescriptorSetsAllocError,

        VulkanTexImageCreateError,
        VulkanTexImageViewCreateError,
        VulkanTexSamplerCreateError
    };

    class EngineErrorCategory : public std::error_category {
    public:
        const char* name() const noexcept override {
            return "EngineError";
        }
        std::string message(int ev) const override {
            switch (static_cast<EngineError>(ev)) {
            case EngineError::Success:
                return "Operation completed successfully";
            case EngineError::ShaderFileLoadError:
                return "Couldn't load shader file";
            case EngineError::ShaderSpirvReflectError:
                return "Couldn't get shader data from SPIR-V code";
            case EngineError::VulkanDeviceNotFound:
                return "No suitable Vulkan physical device found";
            case EngineError::VulkanNotSupported:
                return "Vulkan is not supported";
            case EngineError::VulkanOutOfMemory:
                return "Failed to allocate memory via VMA";
            case EngineError::VulkanLogicalDeviceInitError:
                return "Failed to initialize Vulkan Logical Device";
            case EngineError::VulkanSwapChainInitError:
                return "Failed to initialize Vulkan Swap Chain";
            case EngineError::VulkanImageViewInitError:
                return "Failed to initialize Vulkan Image Views";
            case EngineError::VulkanRenderPassInitError:
                return "Failed to initialize Vulkan Render Pass";
            case EngineError::VulkanFramebufferInitError:
                return "Failed to initialize Vulkan Frame Buffers";
            case EngineError::VulkanCommandPoolInitError:
                return "Failed to initialize Vulkan Command Pool";
            case EngineError::VulkanCommandBufferAllocError:
                return "Failed to allocate Vulkan Command Buffers";
            case EngineError::VulkanSyncObjectsInitError:
                return "Failed to initialize Vulkan synchronization objects";

            case EngineError::VulkanShaderCreateError:
                return "Failed to initialize Vulkan synchronization objects";
            case EngineError::VulkanPipelineCreateError:
                return "Failed to create Vulkan Pipeline";
            case EngineError::VulkanDescriptorSetLayoutError:
                return "Failed to create Vulkan DescriptorSetLayout";
            case EngineError::VulkanPipelineLayoutError:
                return "Failed to create Vulkan PipelineLayout";
            case EngineError::VulkanDescriptorPoolError:
                return "Failed to create Vulkan Descriptor Pool";
            case EngineError::VulkanDescriptorSetsAllocError:
                return "Failed to allocate Vulkan Descriptor Sets";

            case EngineError::VulkanTexImageCreateError:
                return "Failed to create Vulkan image for texture";
            case EngineError::VulkanTexImageViewCreateError:
                return "Failed to create Vulkan image view for texture";
            case EngineError::VulkanTexSamplerCreateError:
                return "Failed to create Vulkan sampler for texture";
            default:
                return "Unknown engine error";
            }
        }
    };

    inline const std::error_category& get_engine_category() {
        static EngineErrorCategory instance;
        return instance;
    }
    inline std::error_code make_error_code(EngineError e) {
        return { static_cast<int>(e), get_engine_category() };
    }
}

namespace std {
    template<>
    struct is_error_code_enum<Engine::Errors::EngineError> : std::true_type {};
}