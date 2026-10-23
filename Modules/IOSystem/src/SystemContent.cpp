#include "IOSystem/SystemContext.h"
#include <iostream>
#include <format>

void DisplayController::update_resolution(int width, int height) {
    this->height = height;
    this->width = width;
    ratio = (float)width / (float)height;
    double_x_ratio = 1 / (2 * ratio);
}

void DisplayController::setup_monitor_video_modes() {
    primary_monitor = glfwGetPrimaryMonitor();
    modes = glfwGetVideoModes(primary_monitor, &modes_count);
    if (!modes) throw std::runtime_error("Can't load monitor video modes");

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

std::string& SystemContext::get_current_system_time() {
    time_str = std::format("{:%Y-%m-%d %H:%M:%S}", sys_clock.now());
    return time_str;
}