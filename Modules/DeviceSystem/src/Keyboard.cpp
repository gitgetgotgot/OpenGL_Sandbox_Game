#include "DeviceSystem/Keyboard.h"
#define GLFW_INCLUDE_NONE //don't use old OpenGL headers
#include <GLFW/glfw3.h>

bool Keyboard::key_is_pressed(uint16_t key) const {
    return !key_states_prev[key] && key_states_curr[key];
}

bool Keyboard::key_is_released(uint16_t key) const {
    return key_states_prev[key] && !key_states_curr[key];
}

bool Keyboard::key_is_held(uint16_t key) const {
    return key_states_prev[key] && key_states_curr[key];
}

bool Keyboard::key_is_idle(uint16_t key) const {
    return !key_states_prev[key] && !key_states_curr[key];
}

const std::vector<uint32_t>& Keyboard::get_pressed_chars() const {
    return pressed_chars;
}

void Keyboard::map_shift_keys() {
    std::fill(shift_key_map, shift_key_map + 97, '\0');
    shift_key_map[GLFW_KEY_1] = '!';
    shift_key_map[GLFW_KEY_2] = '@';
    shift_key_map[GLFW_KEY_3] = '#';
    shift_key_map[GLFW_KEY_4] = '$';
    shift_key_map[GLFW_KEY_5] = '%';
    shift_key_map[GLFW_KEY_6] = '^';
    shift_key_map[GLFW_KEY_7] = '&';
    shift_key_map[GLFW_KEY_8] = '*';
    shift_key_map[GLFW_KEY_9] = '(';
    shift_key_map[GLFW_KEY_0] = ')';
    shift_key_map[GLFW_KEY_MINUS] = '_';
    shift_key_map[GLFW_KEY_EQUAL] = '+';
    shift_key_map[GLFW_KEY_LEFT_BRACKET] = '{';
    shift_key_map[GLFW_KEY_RIGHT_BRACKET] = '}';
    shift_key_map[GLFW_KEY_BACKSLASH] = '|';
    shift_key_map[GLFW_KEY_SEMICOLON] = ':';
    shift_key_map[GLFW_KEY_APOSTROPHE] = '"';
    shift_key_map[GLFW_KEY_GRAVE_ACCENT] = '~';
    shift_key_map[GLFW_KEY_COMMA] = '<';
    shift_key_map[GLFW_KEY_PERIOD] = '>';
    shift_key_map[GLFW_KEY_SLASH] = '?';
}