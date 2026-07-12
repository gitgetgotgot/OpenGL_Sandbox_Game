#include <IOSystem/InputHandler.h>

void InputHandler::setGLFWwindowCallbacks(GLFWwindow* window) {
	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, mouse_wheel_callback);
}

void InputHandler::glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
	SystemContext::screen.update_resolution(width, height);
	glViewport(0, 0, width, height);
}

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((mods & GLFW_MOD_SHIFT) && shiftKeyMap.find(key) != shiftKeyMap.end()) {
		SystemContext::keyBoard.keyStatesCurr[int(shiftKeyMap[key])] = action == GLFW_PRESS || action == GLFW_REPEAT;
	}
	else
		SystemContext::keyBoard.keyStatesCurr[key] = glfwGetKey(window, key) == GLFW_PRESS;
}

void InputHandler::char_callback(GLFWwindow* window, unsigned int codepoint) {
	SystemContext::keyBoard.currentPressedChars.emplace_back((uint32_t)codepoint);
}

void InputHandler::cursor_position_callback(GLFWwindow* window, double mX, double mY) {
	SystemContext::mouse.x_pos = (float)mX;
	SystemContext::mouse.y_pos = SystemContext::screen.height - (float)mY;

	SystemContext::mouse.delta_x = mX - SystemContext::mouse.last_x_pos;
	SystemContext::mouse.delta_y = mY - SystemContext::mouse.last_y_pos;
	SystemContext::mouse.last_x_pos = mX;
	SystemContext::mouse.last_y_pos = mY;
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	SystemContext::mouse.lb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	SystemContext::mouse.rb_curr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

void InputHandler::mouse_wheel_callback(GLFWwindow* window, double xOffset, double yOffset) {
	SystemContext::mouse.wheel_offset = (float)yOffset;
}
