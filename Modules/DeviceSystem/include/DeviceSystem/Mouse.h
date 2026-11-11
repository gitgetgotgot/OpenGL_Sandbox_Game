#pragma once
#include <cstdint>

class Mouse {
public:
    void get_mouse_ortho_coords(uint32_t screen_width, uint32_t screen_height, float screen_ratio);

    bool lb_is_pressed() const { return !lb_prev && lb_curr; }
    bool lb_is_released() const { return lb_prev && !lb_curr; }
    bool lb_is_held() const { return lb_prev && lb_curr; }
    bool lb_is_idle() const { return !lb_prev && !lb_curr; }

    bool rb_is_pressed() const { return !rb_prev && rb_curr; }
    bool rb_is_released() const { return rb_prev && !rb_curr; }
    bool rb_is_held() const { return rb_prev && rb_curr; }
    bool rb_is_idle() const { return !rb_prev && !rb_curr; }

    double x_pos = 0, y_pos = 0;
    double last_x_pos = 0, last_y_pos = 0;
    double delta_x = 0, delta_y = 0;
    double wheel_offset = 0;

    bool lb_curr = false, lb_prev = false;
    bool rb_curr = false, rb_prev = false;

    float ortho_x_pos = 0, ortho_y_pos = 0;
    int world_x_pos = 0, world_y_pos = 0;

    bool blocked_by_UI = false;
};