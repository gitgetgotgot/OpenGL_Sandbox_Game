#pragma once
#include <system_error>
#include <string>
#include <cstdint>

namespace Engine::Errors {
    enum class EngineError : uint8_t {
        Success = 0,
        RendererInitFailed,
        VulkanDeviceNotFound,
        OutOfMemory
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
            case EngineError::RendererInitFailed:
                return "Failed to initialize graphics renderer";
            case EngineError::VulkanDeviceNotFound:
                return "No suitable Vulkan physical device found";
            case EngineError::OutOfMemory:
                return "Failed to allocate memory via VMA";
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