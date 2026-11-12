#pragma once
#include <cstdint>
#include <chrono>
#include <string>
#include <system_error>
#include "DeviceSystem/Display.h"
#include "DeviceSystem/Keyboard.h"
#include "DeviceSystem/Mouse.h"

namespace Engine::Graphics { class Renderer; }
struct GLFWwindow;

class DeviceContext {
public:
    enum GraphicsAPI : uint8_t {
        OPEN_GL, VULKAN, DIRECTX
    };
    static bool init_main_window();
    static bool init_renderer();
    static void setup_GLFW_callbacks();
    static void toggle_main_window(bool state);
    static void destroy_main_window();
    static bool main_window_should_close();
    static void close_main_window();

    static void poll_events();
    static void process_end_input();
    static void present_frame(std::error_code& ec);

    static std::string& get_current_system_time();
    static void exit_application();
    static bool should_exit();
private:
    DeviceContext() = default;
    ~DeviceContext() = default;
    static bool create_vulkan_instance();

    static void glfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height);
    static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void char_callback(GLFWwindow* window, unsigned int codepoint);
    static void cursor_position_callback(GLFWwindow* window, double mX, double mY);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset);
public:
    static GraphicsAPI graphics_API;
    inline static DisplayController display{};
    inline static Keyboard keyBoard{};
    inline static Mouse mouse{};
private:
    inline static std::unique_ptr<Engine::Graphics::Renderer> renderer{};
private:
    static bool should_exit_application;
    static std::chrono::system_clock sys_clock;
    static std::string time_str;

    // Vulkan specific data struct
    struct VulkanInitData;
    static VulkanInitData vulkan_init_data;

    // DirectX specific data struct
    struct DirectXInitData;
    static DirectXInitData directX_init_data;
};