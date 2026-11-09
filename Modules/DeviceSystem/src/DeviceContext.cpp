#include "DeviceSystem/DeviceContext.h"
#include <Utility/Logger.h>
#include <format>
#include <glad/gl.h>
#include <Rendering/OpenGL_Renderer.h>

bool DisplayController::setup_monitor_video_modes() {
    primary_monitor = glfwGetPrimaryMonitor();
    modes = glfwGetVideoModes(primary_monitor, &modes_count);
    if (!modes) {
        Engine::Util::Logger::Log("Can't load monitor video modes");
        return false;
    }

    video_modes.emplace_back();
    for (int i = 0; i < modes_count; i++) {
        const GLFWvidmode& mode = modes[i];
        if (mode.redBits < 8 || mode.greenBits < 8 || mode.blueBits < 8) {
            continue;
        }
        DisplayVideoMode& last_mode = video_modes.back();
        if (last_mode.width == mode.width && last_mode.height == mode.height && last_mode.max_refresh_rate < mode.refreshRate) {
            last_mode.max_refresh_rate = mode.refreshRate;
            last_mode.video_mode_index = i;
        }
        else {
            video_modes.emplace_back(DisplayVideoMode{ mode.width, mode.height, mode.refreshRate, i });
        }
    }
    video_modes.erase(video_modes.begin());

    return true;
}

void DisplayController::update_resolution(int width, int height) {
    this->height = height;
    this->width = width;
    ratio = (float)width / (float)height;
    double_x_ratio = 1 / (2 * ratio);
}

void DisplayController::toggle_fullscreen(bool state) {
    if (is_fullscreen == state) return;
    is_fullscreen = state;
    const GLFWvidmode& mode = modes[video_modes[current_video_mode].video_mode_index];

    if (is_fullscreen) {
        // Switch to full screen
        glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode.width, mode.height, mode.refreshRate);
    }
    else {
        // Switch to windowed mode
        glfwSetWindowMonitor(window, nullptr, 100, 100, mode.width, mode.height, mode.refreshRate);
    }
}

void DisplayController::toggle_VSYNC(bool state) {
    if (vsync_on == state) return;
    vsync_on = state;
    glfwSwapInterval(vsync_on ? 1 : 0);
}

void DisplayController::change_resolution(bool next) {
    if (next) {
        current_video_mode++;
        if (current_video_mode == video_modes.size()) {
            current_video_mode = 0;
        }
    }
    else {
        current_video_mode--;
        if (current_video_mode < 0) {
            current_video_mode = video_modes.size() - 1;
        }
    }

    const GLFWvidmode& mode = modes[video_modes[current_video_mode].video_mode_index];
    if (is_fullscreen)
        glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode.width, mode.height, mode.refreshRate);
    else
        glfwSetWindowMonitor(window, nullptr, 100, 100, mode.width, mode.height, mode.refreshRate);
}

void DisplayController::change_resolution(uint32_t width, uint32_t height) {
    uint32_t index = 0;
    uint32_t size = video_modes.size();
    for (; index < size; ++index) {
        DisplayVideoMode& res = video_modes[index];
        if (res.width == width && res.height == height) {
            current_video_mode = index;
            const GLFWvidmode& mode = modes[video_modes[current_video_mode].video_mode_index];
            if (is_fullscreen)
                glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode.width, mode.height, mode.refreshRate);
            else
                glfwSetWindowMonitor(window, nullptr, 100, 100, mode.width, mode.height, mode.refreshRate);
            break;
        }
    }
}

DisplayController::DisplayVideoMode& DisplayController::get_current_video_mode() {
    return video_modes[current_video_mode];
}


void DeviceContext::setup_GLFW_callbacks() {
    glfwSetFramebufferSizeCallback(display.window, glfwFrameBufferSizeCallback);
    //glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);

    glfwSetKeyCallback(display.window, key_callback);
    glfwSetCharCallback(display.window, char_callback);

    glfwSetCursorPosCallback(display.window, cursor_position_callback);
    glfwSetMouseButtonCallback(display.window, mouse_button_callback);
    glfwSetScrollCallback(display.window, mouse_wheel_callback);

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

void DeviceContext::glfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    // here width and height are physical pixels of framebuffer
    display.update_resolution(width, height);
    Engine::Graphics::OpenGL_Renderer::renderer_ptr->setViewport(0, 0, width, height);
}

void DeviceContext::glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
    // here width and height are logical pixels of window
    display.update_resolution(width, height);
    Engine::Graphics::OpenGL_Renderer::renderer_ptr->setViewport(0, 0, width, height);
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