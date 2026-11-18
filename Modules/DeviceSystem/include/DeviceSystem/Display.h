#pragma once
#include <cstdint>
#include <vector>

struct GLFWwindow;
struct GLFWmonitor;
struct GLFWvidmode;

class DisplayController {
    friend class DeviceContext;
public:
    struct DisplayVideoMode {
        int width = 0, height = 0, max_refresh_rate = 0, video_mode_index = 0;
    };
    // PresentMode that has same values as Renderer RHI
    enum class DisplayPresentMode : uint8_t {
        Immediate,	// no Vsync, no limits
        Double,		// classic Vsync, double buffer
        Triple		// triple buffer, low latency
    };
public:
    bool setup_monitor_video_modes();
    void update_resolution_callback(int width, int height);
    void toggle_fullscreen(bool state);
    void change_present_mode();
    void change_resolution(bool next);
    void change_resolution(uint32_t width, uint32_t height);
    DisplayVideoMode& get_current_video_mode();
public:
    uint32_t width = 0, height = 0;
    float ratio = 0;
    float double_x_ratio = 0;
    bool is_fullscreen = false;
    DisplayPresentMode present_mode = DisplayPresentMode::Double;
private:
    GLFWwindow* window = nullptr;
    GLFWmonitor* primary_monitor = nullptr;
    const GLFWvidmode* modes = nullptr;
    int modes_count = 0;
    std::vector<DisplayVideoMode> video_modes{};
    int current_video_mode = 0;
};