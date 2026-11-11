#pragma once
#include <cstdint>
#include <chrono>
#include <string>

class DisplayController;
class Keyboard;
class Mouse;

class DeviceContext {
public:
    enum GraphicsAPI : uint8_t {
        OPEN_GL, VULKAN, DIRECTX
    };
    static void setup_GLFW_callbacks();

    static void exit_application();
    static bool should_exit();

    static bool init_main_window();
    static void toggle_main_window(bool state);
    static void destroy_main_window();
    static bool main_window_should_close();
    static void close_main_window();

    static void poll_events();
    static void process_end_input();
    static void present_end_frame();

    static std::string& get_current_system_time();
private:
    DeviceContext() = default;
    ~DeviceContext() = default;
public:
    static GraphicsAPI graphics_API;
    static DisplayController display;
    static Keyboard keyBoard;
    static Mouse mouse;
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