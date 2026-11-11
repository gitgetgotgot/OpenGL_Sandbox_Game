#pragma once
#include <cstdint>
#include <vector>

enum Key : uint16_t;

class Keyboard {
public:
    bool key_is_pressed(uint16_t key) const;
    bool key_is_released(uint16_t key) const;
    bool key_is_held(uint16_t key) const;
    bool key_is_idle(uint16_t key) const;
    std::vector<uint32_t> currentPressedChars{};
    bool keyStatesCurr[349]{};
    bool keyStatesPrev[349]{};
    inline static constexpr uint32_t SHIFT_KEY_SIZE = 97;
    inline static char shift_key_map[SHIFT_KEY_SIZE]; //keymap for special keys with shift (last GLFW_ID is 96)
};