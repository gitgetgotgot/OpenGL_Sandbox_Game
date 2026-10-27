#include <Core/GameCore.h>
#include <iostream>

int main() {
	GameEngine game;
	try {
		game.init();
	}
	catch (const std::exception& e) {
		std::cerr << "[INIT]: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	game.main_loop();
}