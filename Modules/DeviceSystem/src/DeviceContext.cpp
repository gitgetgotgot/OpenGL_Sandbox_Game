#include "DeviceSystem/DeviceContext.h"
#include <Base/Config.h>

// define GLFW macros based on OS
#define GLFW_INCLUDE_NONE   // don't use old OpenGL headers
#define GLFW_INCLUDE_VULKAN // use vulkan macro for vulkan instance & surface
#if defined(ENGINE_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32 // use native macro to get win32 window handle
#endif

// include base GLFW header
#include <GLFW/glfw3.h>

// include add-on windows GLFW header
#if defined(ENGINE_PLATFORM_WINDOWS)
#include <GLFW/glfw3native.h>
#endif

// include renderers based on OS
#include <Rendering/OpenGL/gl_Renderer.h>
#include <Rendering/Vulkan/vulkan_Renderer.h>
#if defined(ENGINE_PLATFORM_WINDOWS)
#include <Rendering/DirectX12/d3d12_Renderer.h>
#endif

#include <Utility/Logger.h>
#include <glad/gl.h>
#include <format>

void DeviceContext::glfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    // here width and height are physical pixels of framebuffer
    display.update_resolution(width, height);
    Engine::Graphics::Renderer::instance->setViewport(0, 0, width, height);
}

void DeviceContext::glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    // here width and height are logical pixels of window
    display.update_resolution(width, height);
    Engine::Graphics::Renderer::instance->setViewport(0, 0, width, height);
}

void DeviceContext::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((mods & GLFW_MOD_SHIFT) && key >= 0 && key < keyBoard.SHIFT_KEY_SIZE && keyBoard.shift_key_map[key] != '\0') {
        keyBoard.keyStatesCurr[int(keyBoard.shift_key_map[key])] = action == GLFW_PRESS || action == GLFW_REPEAT;
    }
    else
        keyBoard.keyStatesCurr[key] = glfwGetKey(window, key) == GLFW_PRESS;
}

void DeviceContext::char_callback(GLFWwindow* window, unsigned int codepoint) {
    keyBoard.currentPressedChars.emplace_back((uint32_t)codepoint);
}

void DeviceContext::cursor_position_callback(GLFWwindow* window, double mX, double mY) {
    mouse.x_pos = (float)mX;
    mouse.y_pos = display.height - (float)mY;

    mouse.delta_x = mX - mouse.last_x_pos;
    mouse.delta_y = mY - mouse.last_y_pos;
    mouse.last_x_pos = mX;
    mouse.last_y_pos = mY;
}

void DeviceContext::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    mouse.lb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    mouse.rb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

void DeviceContext::mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset) {
    mouse.wheel_offset = (float)yOffset;
}

void DeviceContext::setup_GLFW_callbacks() {
    glfwSetFramebufferSizeCallback(display.window, glfwFrameBufferSizeCallback);
    //glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);

    glfwSetKeyCallback(display.window, key_callback);
    glfwSetCharCallback(display.window, char_callback);

    glfwSetCursorPosCallback(display.window, cursor_position_callback);
    glfwSetMouseButtonCallback(display.window, mouse_button_callback);
    glfwSetScrollCallback(display.window, mouse_wheel_callback);

    keyBoard.map_shift_keys();
}

bool DeviceContext::init_main_window() {
    if (!glfwInit()) {
        Engine::Util::Logger::Log("Failed to initialize GLFW!");
        return false;
    }

    // hide main window before base game data is loaded
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    if (graphics_API == GraphicsAPI::OPEN_GL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        if (graphics_API == GraphicsAPI::VULKAN) {
            if (create_vulkan_instance() == false) {
                Engine::Util::Logger::Log("Failed to create Vulkan Instance!");
                return false;
            }
        }
    }

    display.window = glfwCreateWindow(1920, 1080, "2D Sandbox", NULL, NULL);
    if (!display.window) {
        Engine::Util::Logger::Log("Failed to create GLFW window!");
        return false;
    }

    if (graphics_API == GraphicsAPI::OPEN_GL) {
        glfwMakeContextCurrent(display.window);
        int version = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
        if (!version) {
            Engine::Util::Logger::Log("Failed to initialize GLAD for OpenGL!");
            return false;
        }
    }
    else if (graphics_API == GraphicsAPI::VULKAN) {
        VkResult result = glfwCreateWindowSurface(vulkan_init_data.instance, display.window, nullptr, &vulkan_init_data.surface);
        if (result != VK_SUCCESS) {
            Engine::Util::Logger::Log("Failed to create Vulkan Window Surface!");
            return false;
        }
    }
    else if (graphics_API == GraphicsAPI::DIRECTX) {
#if defined(ENGINE_PLATFORM_WINDOWS)
        directX_init_data.window_handle = glfwGetWin32Window(display.window);
        if (!directX_init_data.window_handle) {
            Engine::Util::Logger::Log("Failed to get Win32 HWND for DirectX!");
            return false;
        }
#endif
    }

    glfwSetWindowAspectRatio(display.window, 16, 9);
    display.update_resolution(1920, 1080);
    return true;
}

bool DeviceContext::init_renderer() {
    std::error_code ec;
    if (DeviceContext::graphics_API == DeviceContext::GraphicsAPI::OPEN_GL) {
        renderer = std::make_unique<Engine::Graphics::OpenGL_Renderer>(ec);
    }
    else if (DeviceContext::graphics_API == DeviceContext::GraphicsAPI::VULKAN) {
        renderer = std::make_unique<Engine::Graphics::Vulkan_Renderer>(vulkan_init_data.instance, vulkan_init_data.surface, ec);
    }
    else if (DeviceContext::graphics_API == DeviceContext::GraphicsAPI::DIRECTX) {
#if defined(ENGINE_PLATFORM_WINDOWS)
        renderer = std::make_unique<Engine::Graphics::DirectX12_Renderer>(directX_init_data.window_handle, ec);
#endif
    }
    if (ec) {
        Engine::Util::Logger::Log("Failed to initialize Graphics API: " + ec.message());
        return false;
    }
    return true;
}

void DeviceContext::toggle_main_window(bool state) {
    if (state) {
        glfwShowWindow(display.window);
    }
    else {
        glfwHideWindow(display.window);
    }
}

void DeviceContext::destroy_main_window() {
    if (renderer) renderer.reset();
    glfwDestroyWindow(display.window);
    glfwTerminate();
}

bool DeviceContext::main_window_should_close() {
    return glfwWindowShouldClose(display.window);
}

void DeviceContext::close_main_window() {
    glfwSetWindowShouldClose(display.window, true);
}

void DeviceContext::poll_events() {
    glfwPollEvents();
}

void DeviceContext::process_end_input() {
    mouse.lb_prev = mouse.lb_curr;
    mouse.rb_prev = mouse.rb_curr;
    mouse.wheel_offset = 0;
    mouse.delta_x = 0;
    mouse.delta_y = 0;
    mouse.blocked_by_UI = false;

    std::memcpy(keyBoard.keyStatesPrev, keyBoard.keyStatesCurr, 349);

    keyBoard.currentPressedChars.clear();
}

void DeviceContext::present_frame(std::error_code& ec) {
    // for OpenGL use method from GLFW to swap buffers
    if (graphics_API == GraphicsAPI::OPEN_GL) {
        glfwSwapBuffers(display.window);
    }
    // for Vulkan or other use their own swap chain methods
    else {
        Engine::Graphics::Renderer::instance->present_frame(ec);
    }
}

std::string& DeviceContext::get_current_system_time() {
    time_str = std::format("{:%Y-%m-%d %H:%M:%S}", sys_clock.now());
    return time_str;
}

void DeviceContext::exit_application() {
    should_exit_application = true;
}

bool DeviceContext::should_exit() {
    return should_exit_application;
}

bool DeviceContext::create_vulkan_instance() {
    // GLFW extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    // VkInstance creation
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "2D Sandbox";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Custom 2D Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2; // или 1.3 в зависимости от ваших потребностей

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    // Validation Layers can be applied here for Debug (but i don't use them)

    VkResult result = vkCreateInstance(&create_info, nullptr, &vulkan_init_data.instance);
    return result == VK_SUCCESS;
}

DeviceContext::GraphicsAPI DeviceContext::graphics_API = DeviceContext::GraphicsAPI::OPEN_GL;

bool DeviceContext::should_exit_application = false;
std::chrono::system_clock DeviceContext::sys_clock{};
std::string DeviceContext::time_str{};

struct DeviceContext::VulkanInitData {
    VkInstance instance{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };
};

struct DeviceContext::DirectXInitData {
#if defined(ENGINE_PLATFORM_WINDOWS)
    HWND window_handle;
#endif
};

DeviceContext::VulkanInitData DeviceContext::vulkan_init_data{};
DeviceContext::DirectXInitData DeviceContext::directX_init_data{};