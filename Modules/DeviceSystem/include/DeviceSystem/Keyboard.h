#pragma once
#include <cstdint>
#include <vector>

namespace Key {
    // these are mapped from GLFW key values
    enum class Key : int16_t {
        Unknown = 0,

        KeySpace = 32,
        KeyApostrophe = 39,
        KeyComma = 44,
        KeyMinus = 45,
        KeyPeriod = 46,
        KeySlash = 47,

        Key0 = 48,
        Key1 = 49,
        Key2 = 50,
        Key3 = 51,
        Key4 = 52,
        Key5 = 53,
        Key6 = 54,
        Key7 = 55,
        Key8 = 56,
        Key9 = 57,

        KeySemicolon = 59,
        KeyEqual = 61,

        KeyA = 65,
        KeyB = 66,
        KeyC = 67,
        KeyD = 68,
        KeyE = 69,
        KeyF = 70,
        KeyG = 71,
        KeyH = 72,
        KeyI = 73,
        KeyJ = 74,
        KeyK = 75,
        KeyL = 76,
        KeyM = 77,
        KeyN = 78,
        KeyO = 79,
        KeyP = 80,
        KeyQ = 81,
        KeyR = 82,
        KeyS = 83,
        KeyT = 84,
        KeyU = 85,
        KeyV = 86,
        KeyW = 87,
        KeyX = 88,
        KeyY = 89,
        KeyZ = 90,

        KeyLeftBracket = 91,
        KeyBackslash = 92,
        KeyRightBracket = 93,
        KeyGraveAccent = 96,

        KeyEscape = 256,
        KeyEnter = 257,
        KeyTab = 258,
        KeyBackspace = 259,
        KeyInsert = 260,
        KeyDelete = 261,
        KeyRight = 262,
        KeyLeft = 263,
        KeyDown = 264,
        KeyUp = 265,
        KeyPageUp = 266,
        KeyPageDown = 267,
        KeyCapsLock = 280,

        KeyF1 = 290,
        KeyF2 = 291,
        KeyF3 = 292,
        KeyF4 = 293,
        KeyF5 = 294,
        KeyF6 = 295,
        KeyF7 = 296,
        KeyF8 = 297,
        KeyF9 = 298,
        KeyF10 = 299,
        KeyF11 = 300,
        KeyF12 = 301,

        KeyLeftShift = 340,
        KeyLeftCtrl = 341,
        KeyLeftAlt = 342,
        KeyRightShift = 344,
        KeyRightCtrl = 345,
        KeyRightAlt = 346,
    };
}

class Keyboard {
    friend class DeviceContext;
public:
    bool key_is_pressed(uint16_t key) const;
    bool key_is_released(uint16_t key) const;
    bool key_is_held(uint16_t key) const;
    bool key_is_idle(uint16_t key) const;
private:
    void map_shift_keys();
    std::vector<uint32_t> currentPressedChars{};
    bool keyStatesCurr[349]{};
    bool keyStatesPrev[349]{};
    inline static constexpr uint32_t SHIFT_KEY_SIZE = 97;
    inline static char shift_key_map[SHIFT_KEY_SIZE]; //keymap for special keys with shift (last GLFW_ID is 96)
};