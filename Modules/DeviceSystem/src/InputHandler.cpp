#include "DeviceSystem/InputHandler.h"

void InputHandler::setGLFWwindowCallbacks(GLFWwindow* window) {
	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, mouse_wheel_callback);
}

void InputHandler::glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
	DeviceContext::display.update_resolution(width, height);
	glViewport(0, 0, width, height);
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((mods & GLFW_MOD_SHIFT) && shiftKeyMap.find(key) != shiftKeyMap.end()) {
		DeviceContext::keyBoard.keyStatesCurr[int(shiftKeyMap[key])] = action == GLFW_PRESS || action == GLFW_REPEAT;
	}
	else
		DeviceContext::keyBoard.keyStatesCurr[key] = glfwGetKey(window, key) == GLFW_PRESS;
}

void InputHandler::char_callback(GLFWwindow* window, unsigned int codepoint) {
	DeviceContext::keyBoard.currentPressedChars.emplace_back((uint32_t)codepoint);
}

void InputHandler::cursor_position_callback(GLFWwindow* window, double mX, double mY) {
	DeviceContext::mouse.x_pos = (float)mX;
	DeviceContext::mouse.y_pos = DeviceContext::display.height - (float)mY;

	DeviceContext::mouse.delta_x = mX - DeviceContext::mouse.last_x_pos;
	DeviceContext::mouse.delta_y = mY - DeviceContext::mouse.last_y_pos;
	DeviceContext::mouse.last_x_pos = mX;
	DeviceContext::mouse.last_y_pos = mY;
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	DeviceContext::mouse.lb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	DeviceContext::mouse.rb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

void InputHandler::mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset) {
	DeviceContext::mouse.wheel_offset = (float)yOffset;
}