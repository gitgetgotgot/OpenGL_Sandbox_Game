#include <Core/GameCore.h>

//glfwSwapInterval(0); - remove FPS limiter

// VAO - vertex array object(to use different VBOs), VBO - vertex buffer object(for vertices), EBO - element buffer object (for indices)
// VAO first, then VBO, then EBO!
// Unbind in the same way

// local -> world (model matrix)
// world -> view (view matrix)
// view -> clip (projection matrix)
// clip -> screen
// Vclip = Mprojection * Mview * Mmodel * Vlocal

int main() {
	Game game;
	game.init();
	game.main_loop();
	game.uninit();
}