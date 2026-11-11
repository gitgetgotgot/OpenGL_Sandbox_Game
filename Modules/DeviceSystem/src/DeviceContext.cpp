#include "DeviceSystem/DeviceContext.h"
#include "DeviceSystem/Display.h"
#include "DeviceSystem/Keyboard.h"
#include "DeviceSystem/Mouse.h"

#include <Rendering/Interfaces/Renderer.h>
#include <Utility/Logger.h>
#include "DeviceSystem/GLFW_Core.h"
#include <vulkan/vulkan.h>
#include <glad/gl.h>

#include <format>

namespace Engine::GLFWCallbacks {
    static void glfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
        // here width and height are physical pixels of framebuffer
        DeviceContext::display.update_resolution(width, height);
        Engine::Graphics::Renderer::instance->setViewport(0, 0, width, height);
    }

    static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
        // here width and height are logical pixels of window
        DeviceContext::display.update_resolution(width, height);
        Engine::Graphics::Renderer::instance->setViewport(0, 0, width, height);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if ((mods & GLFW_MOD_SHIFT) && key >= 0 && key < DeviceContext::keyBoard.SHIFT_KEY_SIZE && DeviceContext::keyBoard.shift_key_map[key] != '\0') {
            DeviceContext::keyBoard.keyStatesCurr[int(DeviceContext::keyBoard.shift_key_map[key])] = action == GLFW_PRESS || action == GLFW_REPEAT;
        }
        else
            DeviceContext::keyBoard.keyStatesCurr[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }

    static void char_callback(GLFWwindow* window, unsigned int codepoint) {
        DeviceContext::keyBoard.currentPressedChars.emplace_back((uint32_t)codepoint);
    }

    static void cursor_position_callback(GLFWwindow* window, double mX, double mY) {
        DeviceContext::mouse.x_pos = (float)mX;
        DeviceContext::mouse.y_pos = DeviceContext::display.height - (float)mY;

        DeviceContext::mouse.delta_x = mX - DeviceContext::mouse.last_x_pos;
        DeviceContext::mouse.delta_y = mY - DeviceContext::mouse.last_y_pos;
        DeviceContext::mouse.last_x_pos = mX;
        DeviceContext::mouse.last_y_pos = mY;
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        DeviceContext::mouse.lb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        DeviceContext::mouse.rb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    }

    static void mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset) {
        DeviceContext::mouse.wheel_offset = (float)yOffset;
    }
}

void DeviceContext::setup_GLFW_callbacks() {
    glfwSetFramebufferSizeCallback(display.window, Engine::GLFWCallbacks::glfwFrameBufferSizeCallback);
    //glfwSetWindowSizeCallback(window, Engine::GLFWCallbacks::glfwWindowSizeCallback);

    glfwSetKeyCallback(display.window, Engine::GLFWCallbacks::key_callback);
    glfwSetCharCallback(display.window, Engine::GLFWCallbacks::char_callback);

    glfwSetCursorPosCallback(display.window, Engine::GLFWCallbacks::cursor_position_callback);
    glfwSetMouseButtonCallback(display.window, Engine::GLFWCallbacks::mouse_button_callback);
    glfwSetScrollCallback(display.window, Engine::GLFWCallbacks::mouse_wheel_callback);

    std::fill(keyBoard.shift_key_map, keyBoard.shift_key_map + 97, '\0');
    keyBoard.shift_key_map[GLFW_KEY_1] = '!';
    keyBoard.shift_key_map[GLFW_KEY_2] = '@';
    keyBoard.shift_key_map[GLFW_KEY_3] = '#';
    keyBoard.shift_key_map[GLFW_KEY_4] = '$';
    keyBoard.shift_key_map[GLFW_KEY_5] = '%';
    keyBoard.shift_key_map[GLFW_KEY_6] = '^';
    keyBoard.shift_key_map[GLFW_KEY_7] = '&';
    keyBoard.shift_key_map[GLFW_KEY_8] = '*';
    keyBoard.shift_key_map[GLFW_KEY_9] = '(';
    keyBoard.shift_key_map[GLFW_KEY_0] = ')';
    keyBoard.shift_key_map[GLFW_KEY_MINUS] = '_';
    keyBoard.shift_key_map[GLFW_KEY_EQUAL] = '+';
    keyBoard.shift_key_map[GLFW_KEY_LEFT_BRACKET] = '{';
    keyBoard.shift_key_map[GLFW_KEY_RIGHT_BRACKET] = '}';
    keyBoard.shift_key_map[GLFW_KEY_BACKSLASH] = '|';
    keyBoard.shift_key_map[GLFW_KEY_SEMICOLON] = ':';
    keyBoard.shift_key_map[GLFW_KEY_APOSTROPHE] = '"';
    keyBoard.shift_key_map[GLFW_KEY_GRAVE_ACCENT] = '~';
    keyBoard.shift_key_map[GLFW_KEY_COMMA] = '<';
    keyBoard.shift_key_map[GLFW_KEY_PERIOD] = '>';
    keyBoard.shift_key_map[GLFW_KEY_SLASH] = '?';
}

bool DeviceContext::init_main_window() {
    if (!glfwInit()) {
        Engine::Util::Logger::Log("Failed to initialize GLFW!");
        return false;
    }

    if (graphics_API == GraphicsAPI::OPEN_GL) {
        // OpenGL Core 4.6
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // using the core profile for only modern functions
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    // create main window and hide before minimum game data is loaded
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    display.window = glfwCreateWindow(1920, 1080, "2D Sandbox", NULL, NULL);
    if (!display.window) {
        Engine::Util::Logger::Log("Failed to create GLFW window!");
        return false;
    }

    glfwMakeContextCurrent(display.window);
    glfwSetWindowAspectRatio(display.window, 16, 9);

    display.update_resolution(1920, 1080);

    if (graphics_API == GraphicsAPI::OPEN_GL) {
        // initialize GLAD for OpenGL
        int version = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
        if (!version) {
            Engine::Util::Logger::Log("Failed to initialize GLAD for OpenGL!");
            return false;
        }
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

void DeviceContext::exit_application() {
    should_exit_application = true;
}

bool DeviceContext::should_exit() {
    return should_exit_application;
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

void DeviceContext::present_end_frame() {
    glfwSwapBuffers(display.window);
}

std::string& DeviceContext::get_current_system_time() {
    time_str = std::format("{:%Y-%m-%d %H:%M:%S}", sys_clock.now());
    return time_str;
}

DeviceContext::GraphicsAPI DeviceContext::graphics_API = DeviceContext::GraphicsAPI::OPEN_GL;
DisplayController DeviceContext::display{};
Keyboard DeviceContext::keyBoard{};
Mouse DeviceContext::mouse{};

bool DeviceContext::should_exit_application = false;
std::chrono::system_clock DeviceContext::sys_clock{};
std::string DeviceContext::time_str{};

struct DeviceContext::VulkanInitData {
    VkInstance instance{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };
};

struct DeviceContext::DirectXInitData {
    //HWND
};

DeviceContext::VulkanInitData DeviceContext::vulkan_init_data{};
DeviceContext::DirectXInitData DeviceContext::directX_init_data{};