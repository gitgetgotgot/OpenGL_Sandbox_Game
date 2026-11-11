#include "DeviceSystem/Mouse.h"

void Mouse::get_mouse_ortho_coords(uint32_t screen_width, uint32_t screen_height, float screen_ratio) {
    ortho_x_pos = ((static_cast<float>(x_pos) / static_cast<float>(screen_width)) * 2.0f - 1.0f) * screen_ratio;
    ortho_y_pos = (static_cast<float>(y_pos) / static_cast<float>(screen_height)) * 2.0f - 1.0f;
}

bool Mouse::lb_is_pressed() const {
    return !lb_prev && lb_curr;
}

bool Mouse::lb_is_released() const {
    return lb_prev && !lb_curr;
}

bool Mouse::lb_is_held() const {
    return lb_prev && lb_curr;
}

bool Mouse::lb_is_idle() const {
    return !lb_prev && !lb_curr;
}

bool Mouse::rb_is_pressed() const {
    return !rb_prev && rb_curr;
}

bool Mouse::rb_is_released() const {
    return rb_prev && !rb_curr;
}

bool Mouse::rb_is_held() const {
    return rb_prev && rb_curr;
}

bool Mouse::rb_is_idle() const {
    return !rb_prev && !rb_curr;
}