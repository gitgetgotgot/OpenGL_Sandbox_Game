#pragma once
#include <unordered_map>
#include "SystemContext.h"

inline std::unordered_map<int, char> shiftKeyMap = {
	{GLFW_KEY_1, '!'}, {GLFW_KEY_2, '@'}, {GLFW_KEY_3, '#'},
	{GLFW_KEY_4, '$'}, {GLFW_KEY_5, '%'}, {GLFW_KEY_6, '^'},
	{GLFW_KEY_7, '&'}, {GLFW_KEY_8, '*'}, {GLFW_KEY_9, '('},
	{GLFW_KEY_0, ')'}, {GLFW_KEY_MINUS, '_'}, {GLFW_KEY_EQUAL, '+'},
	{GLFW_KEY_LEFT_BRACKET, '{'}, {GLFW_KEY_RIGHT_BRACKET, '}'},
	{GLFW_KEY_BACKSLASH, '|'}, {GLFW_KEY_SEMICOLON, ':'},
	{GLFW_KEY_APOSTROPHE, '"'}, {GLFW_KEY_GRAVE_ACCENT, '~'},
	{GLFW_KEY_COMMA, '<'}, {GLFW_KEY_PERIOD, '>'}, {GLFW_KEY_SLASH, '?'}
};

class InputHandler {
public:
	static void setGLFWwindowCallbacks(GLFWwindow* window);
private:
	//window resize
	static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
	//keyboard
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void char_callback(GLFWwindow* window, unsigned int codepoint);
	//mouse cursor
	static void cursor_position_callback(GLFWwindow* window, double mX, double mY);
	//mouse buttons
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	//mouse wheel scroll
	static void mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset);
};